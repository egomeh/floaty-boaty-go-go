#include "meshrendercomponent.hpp"

TypedComponentHandle MeshRenderLoader::LoadComponent(const nlohmann::json &serializedComponent)
{
    ComponentHandle handle;
    bool success = m_MeshRenderSystem->Create(handle);

    MeshRenderComponent *meshRender = m_MeshRenderSystem->GetComponent(handle);

    std::string meshName = serializedComponent["mesh"];
    std::string materialName = serializedComponent["material"];

    meshRender->mesh = m_AssetDatabase->RequestAsset<Mesh>(meshName);
    meshRender->material = m_AssetDatabase->RequestAsset<Material>(materialName);

    TypedComponentHandle typedHandle;

    typedHandle.componentHandle = handle;
    typedHandle.componentTypeId = GetTargetComponentId();

    return typedHandle;
}

TypedComponentHandle MeshRenderLoader::LoadComponent(ComponentTypeId typeId)
{
    ComponentHandle handle;
    bool success = m_MeshRenderSystem->Create(handle);

    MeshRenderComponent *meshRender = m_MeshRenderSystem->GetComponent(handle);

    TypedComponentHandle typedHandle;

    typedHandle.componentHandle = handle;
    typedHandle.componentTypeId = GetTargetComponentId();

    return typedHandle;
}

ComponentBase *MeshRenderLoader::GetAbstractComponent(ComponentHandle handle)
{
    return m_MeshRenderSystem->GetComponent(handle);
}

void MeshRenderLoader::SetMeshRenderSystem(MeshRenderSystem *meshRenderSystem)
{
    m_MeshRenderSystem = meshRenderSystem;
}
