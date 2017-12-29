#include "transformcomponent.hpp"
#include "serialization/jsonserializer.hpp"

TypedComponentHandle TransformLoader::LoadComponent(const nlohmann::json &serializedComponent)
{
    TypedComponentHandle typedHandle;

    bool success = m_TransformSystem->Create(typedHandle.componentHandle);

    typedHandle.componentTypeId = GetTargetComponentId();

    Transform *transform = m_TransformSystem->GetComponent(typedHandle.componentHandle);

    JsonSerializer serializer(serializedComponent);

    (*transform).Deserialize(serializer);

    return typedHandle;
}

TypedComponentHandle TransformLoader::LoadComponent(ComponentTypeId typeId)
{
    TypedComponentHandle typedHandle;

    bool success = m_TransformSystem->Create(typedHandle.componentHandle);

    typedHandle.componentTypeId = GetTargetComponentId();

    Transform *transform = m_TransformSystem->GetComponent(typedHandle.componentHandle);

    transform->SetLocalPosition(glm::vec3(0, 0, 0));
    transform->SetLocalEulerAngles(glm::vec3(0, 0, 0));

    return typedHandle;
}

ComponentBase *TransformLoader::GetAbstractComponent(ComponentHandle handle)
{
    return m_TransformSystem->GetComponent(handle);
}

void TransformLoader::SetTransformSystem(TransformSystem * transformSystem)
{
    m_TransformSystem = transformSystem;
}
