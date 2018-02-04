#include "definitions.hpp"

#include <sstream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <fstream>
#include <functional>
#include <unordered_set>

#include "glm/glm.hpp"

#include "project_config.h"

#include "application.hpp"
#include "util.hpp"

const wchar_t kClassName[] = L"FloatyBoatyGoGo";

Application *g_ActiveApplication = nullptr;

void Application::DisplayFrameTime()
{
    m_DisplayFrameTime = true;
}

LRESULT CALLBACK Application::WinProcCallback(HWND hwnd, UINT uMSG, WPARAM wParam, LPARAM lParam)
{
    switch (uMSG)
    {
        case WM_CLOSE:
        {
            DebugLog("Close");
            g_ActiveApplication->Stop();
            break;
        }

        case WM_QUIT:
        {
            DebugLog("Quit");
            break;
        }

        case WM_SIZE:
        {
            std::size_t width = LOWORD(lParam);
            std::size_t height = HIWORD(lParam);
            g_ActiveApplication->GetRenderer()->SetViewPort(width, height);
            PostMessage(hwnd, WM_PAINT, 0, 0);
            return 0;
        }

        case WM_KEYDOWN:
        {
            // g_ActiveApplication->DisplayFrameTime();
            // wParam is the key

            if (wParam == VK_ESCAPE)
            {
                g_ActiveApplication->WindowLostFocus();
            }

            break;
        }

        case WM_KEYUP:
        {
            Input::pressedKeys[wParam % 0xff] = true;

            break;
        }

        case WM_ACTIVATE:
        {
            if (wParam == WA_INACTIVE)
            {
                g_ActiveApplication->WindowLostFocus();
            }
            else
            {
                g_ActiveApplication->WindowGainedFocus();
            }

            break;
        }
    }

    return DefWindowProc(hwnd, uMSG, wParam, lParam);
}

void Application::RegisterWindowClass()
{
    WNDCLASSEX windowClass =
    {
        sizeof(WNDCLASSEX),                 // cbSize
        CS_HREDRAW | CS_VREDRAW | CS_OWNDC, // style
        Application::WinProcCallback,       // lpfnWndProc
        0,                                  // cbClsExtra
        0,                                  // cbWndExtra
        m_Instance,                         // hInstance
        0,                                  // hIcon
        LoadCursor(NULL, IDC_ARROW),        // hCursor
        (HBRUSH)(COLOR_WINDOW + 1),         // hbrBackground
        0,                                  // lpszMenuName
        kClassName,                         // lpszClassName
        0                                   // hIconSm
    };

    ATOM registration = RegisterClassEx(&windowClass);

    if (!registration)
    {
        std::stringstream errorMessage;
        errorMessage << "Could not register class with name";
        errorMessage << kClassName << ".";
        throw OSError(errorMessage.str());
    }

    m_ClassRegistered = true;
}

void Application::UnRegisterWindowClass()
{
    if (m_ClassRegistered)
    {
        UnregisterClass(kClassName, m_Instance);
    }

    m_ClassRegistered = false;
}

void Application::CleanupWindow(const wchar_t *className, HGLRC context, HWND window)
{
    if (context)
    {
        wglDeleteContext(context);
    }

    if (window)
    {
        DestroyWindow(window);
    }

    UnregisterClass(className, m_Instance);
}

#ifndef LOAD_RESOURCES_FROM_EXE
void Application::RefreshChangedAssets()
{
    std::vector<FolderModificationEvent> modificationEvents = m_AssetFolderWatcher.GetFolderModificationEvents();

    std::unordered_set<AssetIdentifier> assetsToUpdate;

    for (const FolderModificationEvent &modificationEvent : modificationEvents)
    {
        const std::string &fullFilePath = modificationEvent.fullPath;
        const std::string &topFolder = m_AssetFolderWatcher.GetWatchedFolder();

        const std::size_t relativeSize = fullFilePath.size() - topFolder.size();

        std::string relativePath = fullFilePath.substr(fullFilePath.size() - relativeSize);

        if (relativePath[0] == '/')
        {
            relativePath = relativePath.substr(1);
        }

        const auto fileDependencyList = m_AssetDependencyTracker.GetFileDependencies().find(relativePath);

        if (fileDependencyList != m_AssetDependencyTracker.GetFileDependencies().end())
        {
            const std::vector<AssetIdentifier> &assetIdentifiers = fileDependencyList->second;

            for (const AssetIdentifier &assetIdentifier : assetIdentifiers)
            {
                assetsToUpdate.insert(assetIdentifier);
            }
        }
    }

    while (!assetsToUpdate.empty())
    {
        const AssetIdentifier &assetToUpdate = *assetsToUpdate.begin();

        AssetFactoryBase *assetFactory = m_Assetdatabase.GetAbstractAssetFactory(assetToUpdate.assetType);

        if (assetFactory)
        {
            m_AssetDependencyTracker.UntrackAsset(assetToUpdate);
            assetFactory->RefreshAsset(assetToUpdate.name);

            const auto &fulllAssetDependencies = m_AssetDependencyTracker.GetAssetDependencies();
            const auto &updatedAssetDependencies = fulllAssetDependencies.find(assetToUpdate);

            if (updatedAssetDependencies != fulllAssetDependencies.end())
            {
                for (const AssetIdentifier &triggeredAsset : updatedAssetDependencies->second)
                {
                    assetsToUpdate.insert(triggeredAsset);
                }
            }
        }

        assetsToUpdate.erase(assetToUpdate);
    }
}
#endif

std::string Application::ReadFile(std::string filePath)
{
    std::ifstream fileStream(filePath);

    if (!fileStream.good())
    {
        std::stringstream errorMessage;
        errorMessage << "Could not open " << filePath;
        throw std::runtime_error(errorMessage.str());
    }

    std::istreambuf_iterator<char> it1(fileStream);
    std::istreambuf_iterator<char> it2;
    return std::string(it1, it2);
}

Application *Application::Instance()
{
    return g_ActiveApplication;
}

Application::Application(HINSTANCE hInstance)
    : m_Initialized(false),
    m_ClassRegistered(false),
    m_Instance(hInstance),
    m_Running(false)
{
}

Application::~Application()
{
    UnRegisterWindowClass();
}

void Application::Initialize()
{
    if (g_ActiveApplication != nullptr)
    {
        throw StateError("Active application already set.");
    }

    if (m_Instance == nullptr)
    {
        throw StateError("Initialization may not take place before instance handle is set.");
    }

    m_ActiveOpenGLVersion = OpenGLVersion(0, 0);

    PIXELFORMATDESCRIPTOR pfd;
    ZeroMemory(&pfd, sizeof(pfd));
    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;
    pfd.cDepthBits = 16;
    pfd.cStencilBits = 8;
    pfd.cAlphaBits = 0;
    pfd.iLayerType = PFD_MAIN_PLANE;

    /* Dummy code start */

    const wchar_t dummyClassName[] = L"DummyClass";

    HGLRC dummyContext = 0;
    HWND dummyWindow = 0;
    WNDCLASSEX dummyWindowClass =
    {
        sizeof(WNDCLASSEX),                 // cbSize
        CS_HREDRAW | CS_VREDRAW | CS_OWNDC, // style
        Application::WinProcCallback,       // lpfnWndProc
        0,                                  // cbClsExtra
        0,                                  // cbWndExtra
        m_Instance,                         // hInstance
        0,                                  // hIcon
        LoadCursor(NULL, IDC_ARROW),        // hCursor
        (HBRUSH)(COLOR_WINDOW + 1),         // hbrBackground
        0,                                  // lpszMenuName
        dummyClassName,                     // lpszClassName
        0                                   // hIconSm
    };

    if (!RegisterClassEx(&dummyWindowClass))
    {
        CleanupWindow(dummyClassName, dummyContext, dummyWindow);
        throw OSError("Could not register window class.");
    }

    dummyWindow = CreateWindowEx(
        0,
        dummyClassName,
        L"Dummy Window Text",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        nullptr,
        nullptr,
        m_Instance,
        nullptr
    );

    if (dummyWindow == 0)
    {
        std::string lastApiError = GetLastErrorAsString();
        CleanupWindow(dummyClassName, dummyContext, dummyWindow);
        throw GraphicsError(lastApiError);
    }

    HDC dummyDeviceContext = GetDC(dummyWindow);
    int dummyPixelFormat = ChoosePixelFormat(dummyDeviceContext, &pfd);
    SetPixelFormat(dummyDeviceContext, dummyPixelFormat, &pfd);

    if (dummyPixelFormat == 0)
    {
        std::string lastApiError = GetLastErrorAsString();
        CleanupWindow(dummyClassName, dummyContext, dummyWindow);
        throw GraphicsError(lastApiError);
    }

    dummyContext = wglCreateContext(dummyDeviceContext);

    if (!dummyContext)
    {
        std::string lastApiError = GetLastErrorAsString();
        CleanupWindow(dummyClassName, dummyContext, dummyWindow);
        throw GraphicsError(lastApiError);
    }

    if (!wglMakeCurrent(dummyDeviceContext, dummyContext))
    {
        std::string lastApiError = GetLastErrorAsString();
        CleanupWindow(dummyClassName, dummyContext, dummyWindow);
        throw GraphicsError(lastApiError);
    }

    /* Dummy code end */

    glewExperimental = GL_TRUE;
    GLenum error = glewInit();
    if (error != GLEW_OK)
    {
        throw StateError("Application could not initialize glew.");
    }

    RegisterWindowClass();

    UINT32 styleEx;
    UINT32 style;

    style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
    styleEx = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;

    RECT rect = {0, 0, 480, 640};
    AdjustWindowRectEx(&rect, style, FALSE, styleEx);

    std::string asciiTitle(APP_NAME);
    std::wstring windowTitle(asciiTitle.begin(), asciiTitle.end());

    m_WindowHandle = CreateWindowEx(
        0,
        kClassName,
        windowTitle.data(),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        nullptr,
        nullptr,
        m_Instance,
        nullptr
    );

    if (m_WindowHandle == nullptr)
    {
        UnRegisterWindowClass();
        throw OSError("Unable to create window.");
    }

    m_DeviceContext = GetDC(m_WindowHandle);
    int pixelFormat = ChoosePixelFormat(m_DeviceContext, &pfd);

    if (pixelFormat == 0)
    {
        throw GraphicsError("Could not choose pixel format.");
    }

    if (!SetPixelFormat(m_DeviceContext, pixelFormat, &pfd))
    {
        throw GraphicsError("Could not set pixel format.");
    }

    std::vector<OpenGLVersion> supportedVersions;
    OpenGLVersion gl450(4, 5);
    OpenGLVersion gl440(4, 4);
    OpenGLVersion gl420(4, 2);
    OpenGLVersion gl330(3, 3);
    supportedVersions.push_back(gl450);
    supportedVersions.push_back(gl440);
    supportedVersions.push_back(gl420);
    supportedVersions.push_back(gl330);
    std::sort(supportedVersions.begin(), supportedVersions.end());
    
    std::vector<OpenGLVersion>::reverse_iterator versionIterator;
    versionIterator = supportedVersions.rbegin();

    bool createContextSuccess = false;

    while (versionIterator != supportedVersions.rend())
    {
        std::stringstream version;
        version << "Attempting to make OpenGL ";
        version << versionIterator->GetMajorVersion() << ".";
        version << versionIterator->GetMinorVersion();
        version << " context.";
        DebugLog(version.str());

        int contextAttribs[] =
        {
            WGL_CONTEXT_MAJOR_VERSION_ARB, (int)versionIterator->GetMajorVersion(),
            WGL_CONTEXT_MINOR_VERSION_ARB, (int)versionIterator->GetMinorVersion(),
            WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
#ifdef _DEBUG
            WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_DEBUG_BIT_ARB,
#else
            WGL_CONTEXT_FLAGS_ARB, 0,
#endif
            0
        };

        int pfattribs[] =
        {
            WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
            WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
            WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
            WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
            WGL_COLOR_BITS_ARB, 32,
            WGL_DEPTH_BITS_ARB, 24,
            WGL_STENCIL_BITS_ARB, 8,
            0
        };

        if (wglewIsSupported("WGL_ARB_create_context") == 1)
        {
            m_OpenGLContext = wglCreateContextAttribsARB(m_DeviceContext, nullptr, contextAttribs);

            if (m_OpenGLContext != nullptr)
            {
                wglMakeCurrent(m_DeviceContext, m_OpenGLContext);
                createContextSuccess = true;
                m_ActiveOpenGLVersion = (*versionIterator);
                GLint major, minor;
                glGetIntegerv(GL_MAJOR_VERSION, &major);
                glGetIntegerv(GL_MINOR_VERSION, &minor);
                const GLubyte *glslVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);
                std::stringstream glInfoString; 
                glInfoString << "OpenGL " << major << "." << minor << std::endl;
                glInfoString << "GLSL version: ";
                glInfoString << glslVersion;
                glInfoString << std::endl;
                DebugLog(glInfoString.str());
            }
        }

        if (createContextSuccess)
        {
            break;
        }
        else
        {
            ++versionIterator;
        }
    }

    CleanupWindow(dummyClassName, dummyContext, dummyWindow);

    bool foundSuitableOpenGLVersion = false;
    foundSuitableOpenGLVersion |= m_ActiveOpenGLVersion.GetMajorVersion() != 0;
    foundSuitableOpenGLVersion |= m_ActiveOpenGLVersion.GetMinorVersion() != 0;

    GL_ERROR_CHECK();

    ResourceLoader resourceLoader;
    resourceLoader.Initialize();

    std::string assetMap = resourceLoader.LoadFileAsText("assetmap.json");

    m_Assetdatabase.SetAssetMapping(assetMap);
    m_Assetdatabase.SetReesourceLoader(resourceLoader);

    m_AssetFactories.m_CubemapAssetFactory.SetTextureAssets(m_Assetdatabase.GetAssetSubMap("texture"));

    m_AssetFactories.m_ShaderAssetFactory.SetOpenGLVersion(m_ActiveOpenGLVersion);
    
    // Register asset types
    m_Assetdatabase.RegisterAssetType<SceneData>("scene", &m_AssetFactories.m_SceneAssetFactory);
    m_Assetdatabase.RegisterAssetType<Prefab>("prefab", &m_AssetFactories.m_PrefabAssetFactory);
    m_Assetdatabase.RegisterAssetType<Texture>("texture", &m_AssetFactories.m_TextureAssetFactory);
    m_Assetdatabase.RegisterAssetType<Font>("ttf", &m_AssetFactories.m_FontAssetFactory);
    m_Assetdatabase.RegisterAssetType<FontTexture>("fonttexture", &m_AssetFactories.m_FontTextureAssetFactory);
    m_Assetdatabase.RegisterAssetType<Cubemap>("cubemap", &m_AssetFactories.m_CubemapAssetFactory);
    m_Assetdatabase.RegisterAssetType<GLSL>("glsl", &m_AssetFactories.m_GlslAssetFactory);
    m_Assetdatabase.RegisterAssetType<Shader>("shader", &m_AssetFactories.m_ShaderAssetFactory);
    m_Assetdatabase.RegisterAssetType<Mesh>("mesh", &m_AssetFactories.m_MeshAssetFactory);
    m_Assetdatabase.RegisterAssetType<Material>("material", &m_AssetFactories.m_MaterialAssetFactory);

#ifndef LOAD_ASSETS_FROM_EXE
    m_AssetFolderWatcher.WatchFolderHierarchy(ASSETS_DIR);

    m_AssetFactories.m_SceneAssetFactory.SetAssetDependencyTracker(&m_AssetDependencyTracker);
    m_AssetFactories.m_TextureAssetFactory.SetAssetDependencyTracker(&m_AssetDependencyTracker);
    m_AssetFactories.m_FontAssetFactory.SetAssetDependencyTracker(&m_AssetDependencyTracker);
    m_AssetFactories.m_FontTextureAssetFactory.SetAssetDependencyTracker(&m_AssetDependencyTracker);
    m_AssetFactories.m_CubemapAssetFactory.SetAssetDependencyTracker(&m_AssetDependencyTracker);
    m_AssetFactories.m_GlslAssetFactory.SetAssetDependencyTracker(&m_AssetDependencyTracker);
    m_AssetFactories.m_ShaderAssetFactory.SetAssetDependencyTracker(&m_AssetDependencyTracker);
    m_AssetFactories.m_MeshAssetFactory.SetAssetDependencyTracker(&m_AssetDependencyTracker);
    m_AssetFactories.m_MaterialAssetFactory.SetAssetDependencyTracker(&m_AssetDependencyTracker);
#endif

    if (foundSuitableOpenGLVersion)
    {
        g_ActiveApplication = this;
        m_Initialized = true;

        // First make the renderer
        m_Renderer = std::make_shared<OpenGLRenderer>(m_ActiveOpenGLVersion);
        m_Renderer->SetEntityDatabase(&m_EntityDatabase);
        m_Renderer->SetAssetDatabase(&m_Assetdatabase);
        m_Renderer->SetTransformSystem(&m_TransformSystem);
        m_Renderer->Initialize();

        DragAcceptFiles(m_WindowHandle, TRUE);
    }

    m_ComponentLoaderSystem.SetAssetDatabase(&m_Assetdatabase);

    m_PhysicsSystem.GetSphereColliderSystem().SetEntityDatabase(&m_EntityDatabase);

    // Setup references in component loaders
    m_ComponentLoaders.m_MeshRenderLoader.SetMeshRenderSystem(&m_Renderer->GetMeshRenderSystem());
    m_ComponentLoaders.m_TransformLoader.SetTransformSystem(&m_TransformSystem);
    m_ComponentLoaders.m_CameraLoader.SetCameraSystem(&m_Renderer->GetCameraystem());
    m_ComponentLoaders.m_SphereColliderSystem.SetSphereColliderSystem(&m_PhysicsSystem.GetSphereColliderSystem());

    // Register component loaders
    m_ComponentLoaderSystem.RegisterComponentLoader<MeshRenderComponent>("mesh renderer", &m_ComponentLoaders.m_MeshRenderLoader);
    m_ComponentLoaderSystem.RegisterComponentLoader<Transform>("transform", &m_ComponentLoaders.m_TransformLoader);
    m_ComponentLoaderSystem.RegisterComponentLoader<Camera>("camera", &m_ComponentLoaders.m_CameraLoader);
    m_ComponentLoaderSystem.RegisterComponentLoader<SphereCollider>("sphere collider", &m_ComponentLoaders.m_SphereColliderSystem);

    // This will collect all auto-generated factories for logic components
    m_LogicSystem.Initialize();

    m_LogicSystem.m_EntityDatabase = &m_EntityDatabase;
    m_LogicSystem.m_AssetDatabase = &m_Assetdatabase;
    m_LogicSystem.m_TransformSystem = &m_TransformSystem;

    m_LogicSystem.RegisterWithComponentLoaderSystem(m_ComponentLoaderSystem);

    m_EntityDatabase.RegisterComponentSystem<MeshRenderComponent>(&m_Renderer->GetMeshRenderSystem());
    m_EntityDatabase.RegisterComponentSystem<Camera>(&m_Renderer->GetCameraystem());
    m_EntityDatabase.RegisterComponentSystem<Transform>(&m_TransformSystem);
    m_EntityDatabase.RegisterComponentSystem<SphereCollider>(&m_PhysicsSystem.GetSphereColliderSystem());

    m_LogicSystem.RegisterComponentSystems(&m_EntityDatabase);

    // Check for settings
    if (resourceLoader.HasResource("settings.json"))
    {
        const std::string settingsString = resourceLoader.LoadFileAsText("settings.json");

        m_ApplicationSettings = nlohmann::json::parse(settingsString);
    }
    else
    {
        m_ApplicationSettings = nlohmann::json();
    }
}

void Application::Run()
{
    if (!m_Initialized)
    {
        throw StateError("Attempting to run before initialization.");
    }

    unsigned int startTime = static_cast<unsigned int>(std::time(nullptr));
    std::srand(startTime);

    m_Running = true;

    ShowWindow(m_WindowHandle, 1);
    SetForegroundWindow(m_WindowHandle);
    SetFocus(m_WindowHandle);

    std::string startSceneName = "main";

    if (m_ApplicationSettings.count("start scene") != 0)
    {
        startSceneName = m_ApplicationSettings["start scene"];
    }

    SceneData *mainSceneData = m_Assetdatabase.RequestAsset<SceneData>(startSceneName);

    LoadSceneContent(*mainSceneData, m_EntityDatabase, m_ComponentLoaderSystem);

    m_LogicSystem.CallStartOnLoadedComponents();

    FrameContext frameContext;

    frameContext.deltaTime = 0.f;
    frameContext.deltaMouse = glm::vec2(0.f, 0.f);

    m_PreviousTime = std::clock();
    m_TimeStart = m_PreviousTime;
   
    POINT mousePosition;
    GetCursorPos(&mousePosition);

    m_MousePosition.x = static_cast<float>(mousePosition.x);
    m_MousePosition.y = static_cast<float>(mousePosition.y);

    m_EntityDatabase.SetFrameContext(frameContext);

    Input::enabled = true;

    m_Seconds = 0.;

#ifndef LOAD_ASSETS_FROM_EXE
    for (const auto &dependency : m_AssetDependencyTracker.GetFileDependencies())
    {
        std::stringstream dependencyMessage;

        dependencyMessage << dependency.first << ": ";
        
        for (const auto &dependers : dependency.second)
        {
            dependencyMessage << dependers.name << " ";
        }

        DebugLog(dependencyMessage.str());
    }
#endif

    MSG message;
    while (m_Running)
    {
        const std::size_t frameStartTime = std::clock();

        while (PeekMessage(&message, NULL, 0, 0, PM_REMOVE) > 0)
        {
            TranslateMessage(&message);
            DispatchMessage(&message);
        }

#ifndef LOAD_ASSETS_FROM_EXE
        if (m_AssetFolderWatcher.IsHierarchyModified())
        {
            RefreshChangedAssets();
        }
#endif

        if (!IsIconic(m_WindowHandle) == 0)
        {
            continue;
        }

        m_TransformSystem.Update();

        m_PhysicsSystem.GetSphereColliderSystem().Update();

        auto collisions = m_PhysicsSystem.GetSphereColliderSystem().GetCollisions();

        m_LogicSystem.ResolveCollisions(collisions);

        m_PhysicsSystem.GetSphereColliderSystem().ClearCollisions();

        m_LogicSystem.Update();
        m_LogicSystem.LateUpdate();

        m_TransformSystem.Update();

        m_Renderer->Render();

        m_LogicSystem.CallStartOnLoadedComponents();

        m_EntityDatabase.DestroyMarkedEntities();

        Input::ClearKeyPress();

        POINT mousePosition;
        GetCursorPos(&mousePosition);

        glm::vec2 newMousePosition;
        newMousePosition.x = static_cast<float>(mousePosition.x);
        newMousePosition.y = static_cast<float>(mousePosition.y);

        glm::vec2 deltaMouse = newMousePosition - m_MousePosition;

        m_MousePosition = newMousePosition;

        m_Time = std::clock() - m_TimeStart;

        std::clock_t deltaTime = m_Time - m_PreviousTime;

        m_PreviousTime = m_Time;

        m_Seconds = m_Time / static_cast<double>(CLOCKS_PER_SEC);
        m_DeltaSeconds = deltaTime / static_cast<double>(CLOCKS_PER_SEC);

        m_Renderer->SetTime(m_Seconds);

        if (m_DisplayFrameTime)
        {
            const std::size_t frameEndTime = std::clock();

            const std::size_t totalCPUTime = (frameEndTime - frameStartTime) / (CLOCKS_PER_SEC / 1000);

            double timeInMs = deltaTime / static_cast<double>(CLOCKS_PER_SEC / 1000);

            std::stringstream cpuTime;
            cpuTime << "Frame time (CPU only): " << totalCPUTime << "ms" << std::endl;

            std::stringstream frametimeDisplay;
            frametimeDisplay << "Frame time: " << timeInMs << "ms";
            frametimeDisplay << " / " << (1. / m_DeltaSeconds) << " fps" << std::endl;

            DebugLog(cpuTime.str());
            DebugLog(frametimeDisplay.str());
            m_DisplayFrameTime = false;
        }

        if (m_DeltaSeconds < .1f)
        {
            frameContext.deltaTime =  static_cast<float>(m_DeltaSeconds);
        }
        else
        {
            frameContext.deltaTime = 0.0f;
        }

        if (m_WindowHasFocus)
        {
            frameContext.deltaMouse = deltaMouse;

            RECT windowRect;

            GetWindowRect(m_WindowHandle, &windowRect);

            int cursorX = static_cast<int>(windowRect.left + windowRect.right) / 2;
            int cursorY = static_cast<int>(windowRect.bottom + windowRect.top) / 2;

            SetCursorPos(cursorX, cursorY);

            m_MousePosition.x = static_cast<float>(cursorX);
            m_MousePosition.y = static_cast<float>(cursorY);
        }
        else
        {
            frameContext.deltaMouse = glm::vec2(0.0f, 0.0f);
        }

        m_EntityDatabase.SetFrameContext(frameContext);

        SwapBuffers(m_DeviceContext);

        if (m_DeltaSeconds < 0.001f)
        {
            using namespace std::chrono_literals;

            std::this_thread::sleep_for(1ms);
        }
    }
}

void Application::Stop()
{
    m_Running = false;
}

void Application::WindowGainedFocus()
{
    m_WindowHasFocus = true;

    ShowCursor(false);

    Input::enabled = true;
}

void Application::WindowLostFocus()
{
    m_WindowHasFocus = false;

    ShowCursor(true);

    Input::enabled = false;
}

void Application::DropFile(std::string filePath)
{
}

double Application::GetTotalTime()
{
    return m_Seconds;
}

double Application::GetDeltaTime()
{
    return m_DeltaSeconds;
}

std::shared_ptr<OpenGLRenderer> Application::GetRenderer()
{
    return m_Renderer;
}
