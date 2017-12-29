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
        id = m_ComponentsTemp.size();
        m_ComponentMapping.push_back(static_cast<int>(id));
    }

    MeshRenderComponent newComponent;

    if (insertIntoFreeSlot)
    {
        m_ComponentsTemp[m_ComponentMapping[id]] = newComponent;
    }
    else
    {
        m_ComponentsTemp.push_back(newComponent);
    }

    handle = id;

    return true;
}

bool MeshRenderSystem::CreateComponent(ComponentHandle &handle)
{
    return Create(handle);
}

void MeshRenderSystem::CorrectComponentPlacement(std::size_t id)
{
    std::size_t componentIndex = m_ComponentMapping[id];

    MeshRenderComponent renderer = m_ComponentsTemp[componentIndex];
    RenderQueue renderQueue = renderer.renderQueue;

    // which direction to move the component
    int moveDirection = 0;

    if (m_ComponentMapping[id] == 0)
    {
        moveDirection = 1;
    }
    else if (m_ComponentMapping[id] == m_ComponentsTemp.size() - 1)
    {
        moveDirection = -1;
    }
    else
    {
        RenderQueue nextComponentQueue = m_ComponentsTemp[componentIndex + 1].renderQueue;
        RenderQueue previousComponentQueue = m_ComponentsTemp[componentIndex - 1].renderQueue;

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

        while (swapIndex > 0 && swapIndex < m_ComponentsTemp.size())
        {
            if (!m_ComponentsTemp[swapIndex].IsActive())
            {
                continue;
            }

            if (m_ComponentsTemp[swapIndex].renderQueue * moveDirection < renderQueue * moveDirection)
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

        MeshRenderComponent swappedRenderer = m_ComponentsTemp[swapIndex];

        m_ComponentsTemp[swapIndex] = renderer;
        m_ComponentsTemp[componentIndex] = swappedRenderer;

        m_ComponentMapping[swappedIndex] = static_cast<int>(componentIndex);
        m_ComponentMapping[id] = static_cast<int>(swappedIndex);
    }
}

std::vector<MeshRenderComponent>& MeshRenderSystem::GetComponents()
{
    return m_ComponentsTemp;
}

MeshRenderComponent *MeshRenderSystem::GetComponent(ComponentHandle handle)
{
    if (handle > m_ComponentMapping.size())
    {
        return nullptr;
    }

    MeshRenderComponent &component = m_ComponentsTemp[m_ComponentMapping[handle]];

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
        MeshRenderComponent &component = m_ComponentsTemp[m_ComponentMapping[handle]];

        component.MarkAsDeleted();

        m_FreeComponentSlots.push_back(handle);
    }
}

MeshRenderComponent::MeshRenderComponent()
{
    mesh = nullptr;
    material = nullptr;

    m_Deleted = false;
    m_Active = true;
}

