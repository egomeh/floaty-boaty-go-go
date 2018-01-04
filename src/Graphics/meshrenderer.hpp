#pragma once

#include <vector>

#include "ecs.hpp"
#include "mesh.hpp"
#include "material.hpp"

class MeshRenderComponent : public ComponentBase
{
public:
    MeshRenderComponent(ComponentHandle handle);

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

    void EnsureComponentOrder();

private:
    void SwapInternalComponentLayout(std::size_t a, std::size_t b);

    void CorrectComponentPlacement(std::size_t id);

    std::vector<std::size_t> m_FreeComponentSlots;
    std::vector<int> m_ComponentMapping;
    std::vector<MeshRenderComponent> m_Components;
};
