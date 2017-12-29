#include "spherecollidercomponent.hpp"
#include "serialization/jsonserializer.hpp"

TypedComponentHandle SphereColliderLoader::LoadComponent(const nlohmann::json &serializedComponent)
{
    TypedComponentHandle typedHandle;

    bool success = m_SphereColliderSystem->Create(typedHandle.componentHandle);

    typedHandle.componentTypeId = GetTargetComponentId();

    SphereCollider *sphereCollider = m_SphereColliderSystem->GetComponent(typedHandle.componentHandle);

    JsonSerializer serializer(serializedComponent);

    (*sphereCollider).Deserialize(serializer);

    return typedHandle;
}

TypedComponentHandle SphereColliderLoader::LoadComponent(ComponentTypeId typeId)
{
    TypedComponentHandle typedHandle;

    bool success = m_SphereColliderSystem->Create(typedHandle.componentHandle);

    typedHandle.componentTypeId = GetTargetComponentId();

    SphereCollider *sphereCollider = m_SphereColliderSystem->GetComponent(typedHandle.componentHandle);

    sphereCollider->radius = 1.0f;

    return typedHandle;
}

ComponentBase *SphereColliderLoader::GetAbstractComponent(ComponentHandle handle)
{
    return m_SphereColliderSystem->GetComponent(handle);
}

void SphereColliderLoader::SetSphereColliderSystem(SphereColliderSystem *system)
{
    m_SphereColliderSystem = system;
}
