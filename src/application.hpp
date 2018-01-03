#pragma once

#include <windows.h>
#include <ctime>
#include <memory>

#include "definitions.hpp"
#include "serialization/serialization.hpp"

#include "assetdatabase.hpp"
#include "AssetLoading/assets.hpp"
#include "AssetLoading/assetdependencytracker.hpp"

#include "resources.hpp"
#include "folderwatcher.hpp"

#include "util.hpp"
#include "project_config.h"

#include "ecs.hpp"
#include "transform.hpp"
#include "logic.hpp"

#include "graphics/graphicsutils.hpp"
#include "graphics/graphics.hpp"
#include "graphics/renderer.hpp"
#include "graphics//material.hpp"
#include "graphics/graphics.hpp"
#include "graphics/renderer.hpp"

#include "Physics/physics.hpp"

#include "input.hpp"

class Application
{
public:
    static std::string ReadFile(std::string filePath);
    static Application *Instance();

    Application(HINSTANCE hInstance);
    ~Application();

    void Initialize();
    void Run();
    void Stop();

    void WindowGainedFocus();
    void WindowLostFocus();

    void DropFile(std::string filePath);

    double GetTotalTime();
    double GetDeltaTime();

    std::shared_ptr<OpenGLRenderer> GetRenderer();

    void DisplayFrameTime();

private:
    static LRESULT CALLBACK WinProcCallback(HWND hwnd, UINT uMSG, WPARAM wParam, LPARAM lParam);
    void RegisterWindowClass();
    void UnRegisterWindowClass();
    void CleanupWindow(const wchar_t *className, HGLRC context, HWND window);

private:
    OpenGLVersion m_ActiveOpenGLVersion;

    // Setting data for the application
    nlohmann::json m_ApplicationSettings;

    // WinAPI specifics
    HINSTANCE m_Instance;
    HWND m_WindowHandle;
    HDC m_DeviceContext;
    HGLRC m_OpenGLContext;

    // Flags
    bool m_Initialized;
    bool m_ClassRegistered;
    bool m_Running;
    bool m_DisplayFrameTime;
    bool m_WindowHasFocus;

    // Utility variables
    std::clock_t m_TimeStart;
    std::clock_t m_Time;
    std::clock_t m_PreviousTime;
    double m_Seconds;
    double m_DeltaSeconds;

    // Systems
    TransformSystem m_TransformSystem;
    std::shared_ptr<OpenGLRenderer> m_Renderer;
    PhysicsSystem m_PhysicsSystem;

    // Asset systems
    AssetDatabase m_Assetdatabase;
    AssetFactories m_AssetFactories;

    ComponentLoaderSystem m_ComponentLoaderSystem;
    ComponentLoaderFactories m_ComponentLoaders;

    // ECS
    EntityDatabase m_EntityDatabase;
    LogicSystem m_LogicSystem;

    glm::vec2 m_MousePosition;

#ifndef LOAD_RESOURCES_FROM_EXE
    void RefreshChangedAssets();

    FolderHierarchyWatcher m_AssetFolderWatcher;
    AssetDependencyTracker m_AssetDependencyTracker;
#endif
};
