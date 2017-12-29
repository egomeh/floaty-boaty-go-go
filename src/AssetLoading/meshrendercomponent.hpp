#pragma once

#include "assetdatabase.hpp"
#include "graphics/meshrenderer.hpp"
#include "ecs.hpp"

class MeshRenderLoader : public ComponentLoader<MeshRenderComponent>
{
public:
    TypedComponentHandle LoadComponent(const nlohmann::json &serializedComponent) override;
    TypedComponentHandle LoadComponent(ComponentTypeId typeId) override;
    ComponentBase *GetAbstractComponent(ComponentHandle handle);

    void SetMeshRenderSystem(MeshRenderSystem *meshRenderSystem);

private:
    MeshRenderSystem *m_MeshRenderSystem;
};
