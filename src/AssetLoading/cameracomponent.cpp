#include "cameracomponent.hpp"
#include "serialization/jsonserializer.hpp"

TypedComponentHandle CameraLoader::LoadComponent(const nlohmann::json & serializedComponent)
{
    TypedComponentHandle typedHandle {0, 0};

    bool sucess = m_CameraSystem->Create(typedHandle.componentHandle);

    typedHandle.componentTypeId = GetTargetComponentId();

    Camera *camera = m_CameraSystem->GetCamera();

    JsonSerializer serializer(serializedComponent);

    (*camera).Deserialize(serializer);

    return typedHandle;
}

TypedComponentHandle CameraLoader::LoadComponent(ComponentTypeId typeId)
{
    TypedComponentHandle typedHandle{0, 0};

    bool sucess = m_CameraSystem->Create(typedHandle.componentHandle);

    typedHandle.componentTypeId = GetTargetComponentId();

    Camera *camera = m_CameraSystem->GetCamera();

    return typedHandle;
}

ComponentBase *CameraLoader::GetAbstractComponent(ComponentHandle handle)
{
    return m_CameraSystem->GetCamera();
}

void CameraLoader::SetCameraSystem(CameraSystem *cameraSystem)
{
    m_CameraSystem = cameraSystem;
}
