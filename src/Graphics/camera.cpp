#include "camera.hpp"

CameraSystem::CameraSystem()
{
    m_MainCamera.fov = 60.;
    m_MainCamera.nearPlane = .3f;
    m_MainCamera.nearPlane = 1000.0f;
    m_MainCamera.clearColor = glm::vec4(0., 0., 0., 1.);
}

bool CameraSystem::Create(ComponentHandle &handle)
{
    handle = 0;

    m_MainCamera.assetDatabase = m_AssetDatabase;

    return true;
}

Camera *CameraSystem::GetCamera()
{
    return &m_MainCamera;
}

Camera *CameraSystem::GetComponent(ComponentHandle handle)
{
    return &m_MainCamera;
}

void CameraSystem::DestroyComponent(ComponentHandle handle)
{
}

void CameraSystem::SetAssetDatabase(AssetDatabase *assetDatabase)
{
    m_AssetDatabase = assetDatabase;
}
