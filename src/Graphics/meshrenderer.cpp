#include "meshrenderer.hpp"
#include "graphicsutils.hpp"
#include "shader.hpp"
#include "shaderpreprocessor.hpp"

MeshRenderSystem::MeshRenderSystem()
{
}

bool MeshRenderSystem::Create(ComponentHandle &handle)
{
    std::size_t id = 0;
    bool insertIntoFreeSlot = false;

    if (m_FreeComponentSlots.size() != 0)
    {
        id = m_FreeComponentSlots.back();
        m_FreeComponentSlots.pop_back();
        insertIntoFreeSlot = true;
    }
    else
    {
        id = m_Components.size();
        m_ComponentMapping.push_back(static_cast<int>(id));
    }

    MeshRenderComponent newComponent(id);

    if (insertIntoFreeSlot)
    {
        m_Components[m_ComponentMapping[id]] = newComponent;
    }
    else
    {
        m_Components.push_back(newComponent);
    }

    handle = id;

    return true;
}

bool MeshRenderSystem::CreateComponent(ComponentHandle &handle)
{
    return Create(handle);
}

void MeshRenderSystem::SwapInternalComponentLayout(std::size_t a, std::size_t b)
{
    std::size_t indexA = std::find(m_ComponentMapping.begin(), m_ComponentMapping.end(), a) - m_ComponentMapping.begin();
    std::size_t indexB = std::find(m_ComponentMapping.begin(), m_ComponentMapping.end(), b) - m_ComponentMapping.begin();

    std::swap(m_Components[a], m_Components[b]);
    std::swap(m_ComponentMapping[indexA], m_ComponentMapping[indexB]);
}

void MeshRenderSystem::CorrectComponentPlacement(std::size_t id)
{
    std::size_t componentIndex = m_ComponentMapping[id];

    MeshRenderComponent renderer = m_Components[componentIndex];
    RenderQueue renderQueue = renderer.renderQueue;

    // which direction to move the component
    int moveDirection = 0;

    if (m_ComponentMapping[id] == 0)
    {
        moveDirection = 1;
    }
    else if (m_ComponentMapping[id] == m_Components.size() - 1)
    {
        moveDirection = -1;
    }
    else
    {
        RenderQueue nextComponentQueue = m_Components[componentIndex + 1].renderQueue;
        RenderQueue previousComponentQueue = m_Components[componentIndex - 1].renderQueue;

        if (nextComponentQueue < renderQueue)
        {
            moveDirection = 1;
        }
        else if (previousComponentQueue > renderQueue)
        {
            moveDirection = -1;
        }
    }

    if (moveDirection != 0)
    {
        std::size_t swapIndex = componentIndex;

        while (swapIndex > 0 && swapIndex < m_Components.size())
        {
            if (!m_Components[swapIndex].IsActive())
            {
                continue;
            }

            if (m_Components[swapIndex].renderQueue * moveDirection < renderQueue * moveDirection)
            {
                swapIndex += moveDirection;
            }
            else
            {
                break;
            }
        }

        auto swappedID = std::find(m_ComponentMapping.begin(), m_ComponentMapping.end(), swapIndex);
        auto swappedIndex = std::distance(m_ComponentMapping.begin(), swappedID);

        ASSERT(swappedID != m_ComponentMapping.end(), "Cannot swap with invalid ID");

        MeshRenderComponent swappedRenderer = m_Components[swapIndex];

        m_Components[swapIndex] = renderer;
        m_Components[componentIndex] = swappedRenderer;

        m_ComponentMapping[swappedIndex] = static_cast<int>(componentIndex);
        m_ComponentMapping[id] = static_cast<int>(swappedIndex);
    }
}

std::vector<MeshRenderComponent>& MeshRenderSystem::GetComponents()
{
    return m_Components;
}

MeshRenderComponent *MeshRenderSystem::GetComponent(ComponentHandle handle)
{
    if (handle > m_ComponentMapping.size())
    {
        return nullptr;
    }

    MeshRenderComponent &component = m_Components[m_ComponentMapping[handle]];

    if (!component.IsActive() || component.IsDeleted())
    {
        return nullptr;
    }

    return &component;
}

void MeshRenderSystem::DestroyComponent(ComponentHandle handle)
{
    if (handle < m_ComponentMapping.size())
    {
        MeshRenderComponent &component = m_Components[m_ComponentMapping[handle]];

        component.MarkAsDeleted();

        m_FreeComponentSlots.push_back(handle);
    }
}

void MeshRenderSystem::EnsureComponentOrder()
{
    bool orderIsCorrect = true;

    // i is the component being inspected currently
    for (int i = 0; i < m_Components.size() - 1; ++i)
    {
        MeshRenderComponent &meshRenderer = m_Components[i];
        RenderQueue currentRenderQueue = meshRenderer.renderQueue;

        if (!meshRenderer.IsActive() || meshRenderer.IsDeleted())
        {
            continue;
        }

        // j is the next active component to compare against
        for (int j = i + 1; j < m_Components.size(); ++j)
        {
            MeshRenderComponent &otherMeshRenderer = m_Components[j];
            RenderQueue otherRenderQueue = otherMeshRenderer.renderQueue;

            if (!otherMeshRenderer.IsActive() || otherMeshRenderer.IsDeleted())
            {
                continue;
            }

            if (otherRenderQueue < currentRenderQueue)
            {
                orderIsCorrect = false;
            }
        }
    }

    if (orderIsCorrect)
    {
        return;
    }

    // The ordering is not correct, perform expensive and comprehensive reordering
    // i is the component being inspected currently
    for (int i = 0; i < m_Components.size() - 1; ++i)
    {
        // j is the next active component to compare against
        for (int j = 1; j < m_Components.size(); ++j)
        {
            MeshRenderComponent &meshRenderer = m_Components[j - 1];
            RenderQueue currentRenderQueue = meshRenderer.renderQueue;

            if (!meshRenderer.IsActive() || meshRenderer.IsDeleted())
            {
                continue;
            }

            MeshRenderComponent &otherMeshRenderer = m_Components[j];
            RenderQueue otherRenderQueue = otherMeshRenderer.renderQueue;

            if (!otherMeshRenderer.IsActive() || otherMeshRenderer.IsDeleted())
            {
                continue;
            }

            if (otherRenderQueue < currentRenderQueue)
            {
                SwapInternalComponentLayout(j - 1, j);
            }
        }
    }
}

MeshRenderComponent::MeshRenderComponent(ComponentHandle handle) : renderQueue(RenderQueue::Default)
{
    mesh = nullptr;
    material = nullptr;

    m_Deleted = false;
    m_Active = true;
}
