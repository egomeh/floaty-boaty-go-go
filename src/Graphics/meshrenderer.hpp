#pragma once

#include <vector>

#include "ecs.hpp"
#include "mesh.hpp"
#include "material.hpp"

class MeshRenderComponent : public ComponentBase
{
public:
    MeshRenderComponent();

    Mesh *mesh;
    Material *material;
    RenderQueue renderQueue;
};

class MeshRenderSystem : public ComponentSystem<MeshRenderComponent>
{
public:
    MeshRenderSystem();

    bool Create(ComponentHandle &handle);
    bool CreateComponent(ComponentHandle &handle);

    std::vector<MeshRenderComponent> &GetComponents();
    MeshRenderComponent *GetComponent(ComponentHandle handle) override;

    void DestroyComponent(ComponentHandle handle) override;

private:
    void CorrectComponentPlacement(std::size_t id);

    std::vector<std::size_t> m_FreeComponentSlots;
    std::vector<int> m_ComponentMapping;
    std::vector<MeshRenderComponent> m_ComponentsTemp;
};
