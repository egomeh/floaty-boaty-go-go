#pragma once

#include <string>
#include <cstring>

#include "graphics.hpp"
#include "mesh.hpp"
#include "shader.hpp"

#include "stb.h"

#include "assetdatabase.hpp"
#include "texture.hpp"
#include "material.hpp"
#include "mesh.hpp"
#include "meshrenderer.hpp"
#include "transform.hpp"
#include "camera.hpp"
#include "openglstate.hpp"

#define UNIFORM_NAME(uniformName) std::size_t uniformName = std::hash<std::string>()(#uniformName)

struct CommonUnifroms
{
public:
    UNIFORM_NAME(_EnvironmentMap);
    UNIFORM_NAME(_ObjectToClipspace);
    UNIFORM_NAME(_ObjectToWorld);
    UNIFORM_NAME(_WorldToCamera);
    UNIFORM_NAME(_Perspective);
    UNIFORM_NAME(_NormalMatrix);
    UNIFORM_NAME(_ProjectionParameters);
};

class OpenGLRenderer
{
public:
    OpenGLRenderer(OpenGLVersion openGLVersion);

    void Initialize();
    void SetEntityDatabase(EntityDatabase *entityDatabase);
    void SetAssetDatabase(AssetDatabase *assetDatabase);
    void SetTransformSystem(TransformSystem *transfromSystem);

    void OpenGLRenderer::SetViewPort(std::size_t width, std::size_t height);

    void Render();

    std::size_t GetHeight() const;
    std::size_t GetWidth() const;

    void Blit(Texture *source, RenderTexture *target, Material &material);
    void Blit(Texture *source, RenderTexture *target);

    void SetDefaultUniforms(Material &material);
    void BindAndPrepareShader(const Material &material);

    void SetTime(float time);
    void SetTime(double time);

    static void SetDrawBlendMode(ShaderBlendMode blendMode);

    inline double GetAspectRatio();

    CameraSystem &GetCameraystem();
    MeshRenderSystem &GetMeshRenderSystem();

private:
    inline void GenerateBuiltinMeshes();

    CommonUnifroms m_CommonUniforms;
    OpenGLState m_OpenGLState;

    AssetDatabase *m_AssetDatabase;

    OpenGLVersion m_GLVerison;

    std::size_t m_Width, m_Height;

    float m_Time;

    Mesh m_FullscreenTriangle;
    Mesh m_UnitCube;

    std::shared_ptr<Material> m_BlitMaterial;
    std::shared_ptr<Material> m_SkyboxMaterial;

    RenderTexture m_MainTarget;
    RenderTexture m_PostProcessSwapChain[2];

    CameraSystem m_CameraSystem;
    MeshRenderSystem m_MeshRenderSystem;

    // ECS Related variables
    EntityDatabase *m_EntityDatabase;
    TransformSystem *m_TransformSystem;
};
