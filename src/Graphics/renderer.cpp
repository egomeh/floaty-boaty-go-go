#include <sstream>
#include <memory>
#include "renderer.hpp"

#include "../util.hpp"
#include "graphicsutils.hpp"

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/matrix_inverse.hpp"
#include <math.h>

#include "logic.hpp"

OpenGLRenderer::OpenGLRenderer(OpenGLVersion openGLVersion) :
    m_GLVerison(openGLVersion), m_MainTarget(1, 1, 24), m_SecondTarget(1, 1, 24)
{
    glEnable(GL_MULTISAMPLE);
    GL_ERROR_CHECK();
}

void OpenGLRenderer::Initialize()
{
    m_BlitMaterial = std::make_shared<Material>((m_AssetDatabase->RequestAsset<Shader>("blit")));
    m_SkyboxMaterial = std::make_shared<Material>((m_AssetDatabase->RequestAsset<Shader>("skybox")));

    GenerateBuiltinMeshes();

    m_CameraSystem.SetAssetDatabase(m_AssetDatabase);
}

void OpenGLRenderer::SetEntityDatabase(EntityDatabase * entityDatabase)
{
    m_EntityDatabase = entityDatabase;
}

void OpenGLRenderer::SetAssetDatabase(AssetDatabase *assetDatabase)
{
    m_AssetDatabase = assetDatabase;
}

void OpenGLRenderer::SetTransformSystem(TransformSystem *transformSystem)
{
    m_TransformSystem = transformSystem;
}

void OpenGLRenderer::SetViewPort(std::size_t width, std::size_t height)
{
    m_Width = width;
    m_Height = height;

    m_MainTarget.Release();
    m_SecondTarget.Release();

    m_MainTarget = RenderTexture(m_Width, m_Height, 24);
    m_MainTarget.Create();

    m_SecondTarget = RenderTexture(m_Width, m_Height, 24);
    m_SecondTarget.Create();
}

void OpenGLRenderer::Render()
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_MainTarget.GetFramebufferID());
    GL_ERROR_CHECK();

    glEnable(GL_CULL_FACE);
    GL_ERROR_CHECK();

    glFrontFace(GL_CW);
    GL_ERROR_CHECK();

    glCullFace(GL_BACK);
    GL_ERROR_CHECK();

    glViewport(0, 0, (GLsizei)m_Width, (GLsizei)m_Height);
    GL_ERROR_CHECK();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    GL_ERROR_CHECK();

    glEnable(GL_DEPTH_TEST);
    GL_ERROR_CHECK();

    glDepthFunc(GL_LESS);
    GL_ERROR_CHECK();

    Camera *camera = m_CameraSystem.GetCamera();

    glm::vec4 clearColor = camera->clearColor;

    glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);

    ComponentHandle cameraTransformHandle;
    m_EntityDatabase->FindComponentHandle<Transform>(camera->owner, cameraTransformHandle);

    Transform *cameraTransform = m_TransformSystem->GetComponent(cameraTransformHandle);

    glm::vec3 cameraPosition = cameraTransform->GetLocalPosition();
    glm::vec3 cameraForwards = cameraTransform->GetLocalForwards();
    glm::vec3 cameraUp = cameraTransform->GetLocalUp();

    glm::mat4 projection = glm::perspective(glm::radians(camera->fov), static_cast<float>(GetAspectRatio()), camera->nearPlane, camera->farPlane);
    glm::mat4 view = glm::lookAt(cameraPosition, cameraPosition + cameraForwards, cameraUp);

    float time = m_Time;
    glm::vec3 position(std::sin(.5) * 15., 1., std::cos(.5) * 15.);

    std::size_t _ObjectToClipspace = std::hash<std::string>()("_ObjectToClipspace");
    std::size_t _ObjectToWorld = std::hash<std::string>()("_ObjectToWorld");
    std::size_t _WorldToCamera = std::hash<std::string>()("_WorldToCamera");
    std::size_t _Perspective = std::hash<std::string>()("_Perspective");
    std::size_t _NormalMatrix = std::hash<std::string>()("_NormalMatrix");

    std::size_t _CameraPosition = std::hash<std::string>()("_CameraPosition");

    std::size_t _Skybox = std::hash<std::string>()("_Skybox");

    m_MeshRenderSystem.EnsureComponentOrder();

    std::vector<MeshRenderComponent> &renderers = m_MeshRenderSystem.GetComponents();

    ShaderBlendMode currentBlendMode;

    if (!renderers.empty())
    {
        currentBlendMode = renderers[0].material->GetShader()->GetBlendMode();
        SetDrawBlendMode(currentBlendMode);
    }

    // Draw opaque geometry
    auto renderIterator = renderers.begin();
    for (; renderIterator != renderers.end(); ++renderIterator)
    {
        if (renderIterator->IsDeleted() || !renderIterator->IsActive())
        {
            continue;
        }

        if (renderIterator->renderQueue >= RenderQueue::Transparent)
        {
            break;
        }

        ComponentHandle transformHandle;

        m_EntityDatabase->FindComponentHandle<Transform>(renderIterator->owner, transformHandle);

        Transform *transform = m_TransformSystem->GetComponent(transformHandle);

        glm::mat4x4 model = transform->GetTransformMatrix();

        glm::mat4x4 mvp = projection * view * model;

        glm::mat3 normalMatrix = glm::inverseTranspose(glm::mat3(model));

        Material &material = *(renderIterator->material);

        material.GetUniformContext().SetMat4(_ObjectToClipspace, mvp);
        material.GetUniformContext().SetMat4(_WorldToCamera, view);
        material.GetUniformContext().SetMat4(_Perspective, projection);
        material.GetUniformContext().SetMat4(_ObjectToWorld, model);
        material.GetUniformContext().SetMat3(_NormalMatrix, glm::mat3(normalMatrix));

        material.GetUniformContext().SetVector3(_CameraPosition, cameraPosition);

        material.GetUniformContext().SetTexture(_Skybox, camera->cubemapSkybox);

        material.BindAndPrepareShader();

        SetDefaultUniforms(*renderIterator->material);

        renderIterator->mesh->Bind();
        renderIterator->mesh->Draw();
    }

    // Draw cubemap if needed
    if (camera->clearType == CameraClearType::ClearCubemap)
    {
        glDepthFunc(GL_LEQUAL);
        GL_ERROR_CHECK();

        glm::mat4 skyboxView = glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f) + cameraForwards, cameraUp);

        m_SkyboxMaterial->GetUniformContext().SetMat4(_ObjectToClipspace, projection * skyboxView);
        m_SkyboxMaterial->GetUniformContext().SetTexture(_Skybox, camera->cubemapSkybox);

        m_SkyboxMaterial->BindAndPrepareShader();

        m_UnitCube.Bind();
        m_UnitCube.Draw();
    }

    // Enable transparency and render transparent geometry
    glEnable(GL_DEPTH_TEST);
    GL_ERROR_CHECK();

    glDepthFunc(GL_LESS);
    GL_ERROR_CHECK();

    glEnable(GL_BLEND);
    GL_ERROR_CHECK();

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    GL_ERROR_CHECK();

    for (; renderIterator != renderers.end(); ++renderIterator)
    {
        if (renderIterator->IsDeleted() || !renderIterator->IsActive())
        {
            continue;
        }

        ComponentHandle transformHandle;

        m_EntityDatabase->FindComponentHandle<Transform>(renderIterator->owner, transformHandle);

        Transform *transform = m_TransformSystem->GetComponent(transformHandle);

        glm::mat4x4 model = transform->GetTransformMatrix();

        glm::mat4x4 mvp = projection * view * model;

        glm::mat3 normalMatrix = glm::inverseTranspose(glm::mat3(model));

        Material &material = *(renderIterator->material);

        material.GetUniformContext().SetMat4(_ObjectToClipspace, mvp);
        material.GetUniformContext().SetMat4(_WorldToCamera, view);
        material.GetUniformContext().SetMat4(_Perspective, projection);
        material.GetUniformContext().SetMat4(_ObjectToWorld, model);
        material.GetUniformContext().SetMat3(_NormalMatrix, glm::mat3(normalMatrix));

        material.GetUniformContext().SetVector3(_CameraPosition, cameraPosition);

        material.GetUniformContext().SetTexture(_Skybox, camera->cubemapSkybox);

        material.BindAndPrepareShader();

        SetDefaultUniforms(*renderIterator->material);

        renderIterator->mesh->Bind();
        renderIterator->mesh->Draw();
    }

    glCullFace(GL_BACK);
    GL_ERROR_CHECK();

    glDisable(GL_DEPTH_TEST);
    GL_ERROR_CHECK();

    // Do Post processing

    RenderTexture *source = &m_MainTarget;
    RenderTexture *target = &m_SecondTarget;

    for (const PostProcess &postProcess : camera->postProcessHooks)
    {
        postProcess.logicComponent->OnRenderImage(source, target);
        std::swap(source, target);
    }

    Blit(source, nullptr);
}

std::size_t OpenGLRenderer::GetHeight() const
{
    return m_Height;
}

std::size_t OpenGLRenderer::GetWidth() const
{
    return m_Width;
}

void OpenGLRenderer::SetDefaultUniforms(Material &material)
{
    std::size_t aspectUniform = std::hash<std::string>()("_Aspect");
    std::size_t timeUniform = std::hash<std::string>()("_Time");

    float faspect = static_cast<float>(m_Width) / static_cast<float>(m_Height);

    glm::vec4 time = glm::vec4(m_Time, m_Time, m_Time, m_Time);

    material.GetUniformContext().SetFloat(aspectUniform, faspect);
    material.GetUniformContext().SetVector4(timeUniform, time);
}

void OpenGLRenderer::SetTime(float time)
{
    m_Time = time;
}

void OpenGLRenderer::SetTime(double time)
{
    m_Time = static_cast<float>(time);
}


void OpenGLRenderer::SetDrawBlendMode(ShaderBlendMode blendMode)
{
    if (blendMode == Off)
    {
        glDisable(GL_BLEND);
        GL_ERROR_CHECK();
    }
    else
    {
        glEnable(GL_BLEND);
        GL_ERROR_CHECK();
    }

    switch (blendMode)
    {
        case Add:
        {
            glBlendFunc(GL_SRC_ALPHA, GL_ONE);
            GL_ERROR_CHECK();
            break;
        }
        case Alpha:
        {
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            GL_ERROR_CHECK();
            break;
        }
        case Subtract:
        {
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            GL_ERROR_CHECK();
            break;
        }
        case Multiply:
        {
            glBlendFunc(GL_DST_COLOR, GL_ZERO);
            GL_ERROR_CHECK();
            break;
        }
        default:
        {

        }
    }
}

inline double OpenGLRenderer::GetAspectRatio()
{
    return static_cast<double>(m_Width) / static_cast<double>(m_Height);
}

CameraSystem &OpenGLRenderer::GetCameraystem()
{
    return m_CameraSystem;
}

MeshRenderSystem & OpenGLRenderer::GetMeshRenderSystem()
{
    return m_MeshRenderSystem;
}

void OpenGLRenderer::Blit(Texture *source, RenderTexture *target, Material &material)
{
    if (target)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, target->GetFramebufferID());
        GL_ERROR_CHECK();
    }
    else
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        GL_ERROR_CHECK();
    }

    if (source)
    {
        std::size_t sourceNameHash = std::hash<std::string>()("_MainTex");
        std::size_t sourcePropHash = std::hash<std::string>()("_MainTex_TexelSize");

        float width = static_cast<float>(source->GetWidth());
        float height = static_cast<float>(source->GetHeight());

        material.GetUniformContext().SetTexture(sourceNameHash, source);
        material.GetUniformContext().SetVector4(sourcePropHash, glm::vec4(1. / width, 1. / height, width, height));
    }

    SetDefaultUniforms(material);

    material.BindAndPrepareShader();

    SetDrawBlendMode(material.GetShader()->GetBlendMode());

    m_FullscreenTriangle.Bind();

    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*)0);
    GL_ERROR_CHECK();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    GL_ERROR_CHECK();
}

void OpenGLRenderer::Blit(Texture *source, RenderTexture *target)
{
    Blit(source, target, *m_BlitMaterial.get());
}

inline void OpenGLRenderer::GenerateBuiltinMeshes()
{
    // Generate full screen triangle
    {
        std::shared_ptr<BasicMeshAttribute<glm::vec3>> positionAttribute = std::make_shared<BasicMeshAttribute<glm::vec3>>(Float, 3);
        positionAttribute->AddData(glm::vec3(-1, -1, 1));
        positionAttribute->AddData(glm::vec3(3, -1, 1));
        positionAttribute->AddData(glm::vec3(-1, 3, 1));

        std::shared_ptr<BasicMeshAttribute<glm::vec2>> uvAttribute = std::make_shared<BasicMeshAttribute<glm::vec2>>(Float, 2);
        uvAttribute->AddData(glm::vec2(0, 0));
        uvAttribute->AddData(glm::vec2(2, 0));
        uvAttribute->AddData(glm::vec2(0, 2));

        std::vector<unsigned int> ind{0, 2, 1};

        m_FullscreenTriangle.SetAttribute(0, positionAttribute);
        m_FullscreenTriangle.SetAttribute(1, uvAttribute);
        m_FullscreenTriangle.SetIndecies(ind);

        m_FullscreenTriangle.GenerateDeviceBuffers();
    }
    
    // Generate unit cube, no uv map
    {
        std::vector<glm::vec3> vertices;

        // front face
        vertices.push_back(glm::vec3(-1.0, -1.0, 1.0));
        vertices.push_back(glm::vec3(1.0, -1.0, 1.0));
        vertices.push_back(glm::vec3(-1.0, 1.0, 1.0));

        vertices.push_back(glm::vec3(1.0, -1.0, 1.0));
        vertices.push_back(glm::vec3(1.0, 1.0, 1.0));
        vertices.push_back(glm::vec3(-1.0, 1.0, 1.0));

        // back face
        vertices.push_back(glm::vec3(-1.0, 1.0, -1.0));
        vertices.push_back(glm::vec3(1.0, -1.0, -1.0));
        vertices.push_back(glm::vec3(-1.0, -1.0, -1.0));

        vertices.push_back(glm::vec3(-1.0, 1.0, -1.0));
        vertices.push_back(glm::vec3(1.0, 1.0, -1.0));
        vertices.push_back(glm::vec3(1.0, -1.0, -1.0));


        // left face
        vertices.push_back(glm::vec3(-1.0, -1.0, 1.0));
        vertices.push_back(glm::vec3(-1.0, 1.0, -1.0));
        vertices.push_back(glm::vec3(-1.0, -1.0, -1.0));

        vertices.push_back(glm::vec3(-1.0, -1.0, 1.0));
        vertices.push_back(glm::vec3(-1.0, 1.0, 1.0));
        vertices.push_back(glm::vec3(-1.0, 1.0, -1.0));

        // right face
        vertices.push_back(glm::vec3(1.0, -1.0, 1.0));
        vertices.push_back(glm::vec3(1.0, -1.0, -1.0));
        vertices.push_back(glm::vec3(1.0, 1.0, -1.0));

        vertices.push_back(glm::vec3(1.0, 1.0, -1.0));
        vertices.push_back(glm::vec3(1.0, 1.0, 1.0));
        vertices.push_back(glm::vec3(1.0, -1.0, 1.0));

        // top face
        vertices.push_back(glm::vec3(1.0, 1.0, -1.0));
        vertices.push_back(glm::vec3(-1.0, 1.0, -1.0));
        vertices.push_back(glm::vec3(-1.0, 1.0, 1.0));

        vertices.push_back(glm::vec3(1.0, 1.0, -1.0));
        vertices.push_back(glm::vec3(-1.0, 1.0, 1.0));
        vertices.push_back(glm::vec3(1.0, 1.0, 1.0));

        // bottom face
        vertices.push_back(glm::vec3(-1.0, -1.0, 1.0));
        vertices.push_back(glm::vec3(-1.0, -1.0, -1.0));
        vertices.push_back(glm::vec3(1.0, -1.0, -1.0));

        vertices.push_back(glm::vec3(1.0, -1.0, 1.0));
        vertices.push_back(glm::vec3(-1.0, -1.0, 1.0));
        vertices.push_back(glm::vec3(1.0, -1.0, -1.0));

        std::shared_ptr<BasicMeshAttribute<glm::vec3>> positionAttribute = std::make_shared<BasicMeshAttribute<glm::vec3>>(Float, 3);
        std::vector<unsigned int> indices;
        
        for (const glm::vec3 &vertex : vertices)
        {
            indices.push_back(static_cast<unsigned int>(indices.size()));
            positionAttribute->AddData(vertex);
        }

        m_UnitCube.SetAttribute(0, positionAttribute);
        m_UnitCube.SetIndecies(indices);

        m_UnitCube.GenerateDeviceBuffers();
    }
}
