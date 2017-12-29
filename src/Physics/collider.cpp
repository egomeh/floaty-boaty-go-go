#include "physics.hpp"

bool SphereColliderSystem::Create(ComponentHandle &handle)
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
        id = m_SphereColliders.size();

        m_ComponentMapping.push_back(static_cast<int>(id));
    }

    SphereCollider newComponent;

    if (insertIntoFreeSlot)
    {
        m_SphereColliders[m_ComponentMapping[id]] = newComponent;
    }
    else
    {
        m_SphereColliders.push_back(newComponent);
    }

    handle = id;

    return true;
}

void SphereColliderSystem::Update()
{
    for (auto it = m_SphereColliders.begin(); it != m_SphereColliders.end(); ++it)
    {
        if (it->IsDeleted() || !it->IsActive())
        {
            continue;
        }

        const float radius = it->radius;

        Transform *transform = m_EntityDatabase->GetComponent<Transform>(it->owner);

        const glm::vec3 position = transform->GetTransformMatrix() * glm::vec4(0, 0, 0, 1);

        for (auto itOther = it + 1; itOther != m_SphereColliders.end(); ++itOther)
        {
            if (itOther->IsDeleted() || !itOther->IsActive())
            {
                continue;
            }

            if (it == itOther)
            {
                continue;
            }

            const float radiusOther = itOther->radius;

            Transform *transformOther = m_EntityDatabase->GetComponent<Transform>(itOther->owner);

            const glm::vec3 positionOther = transformOther->GetTransformMatrix() * glm::vec4(0, 0, 0, 1);

            const float distance = glm::length(position - positionOther);

            const float radiusSum = radius + radiusOther;

            if (distance < radiusSum)
            {
                Collision collision;

                collision.body0 = it->owner;
                collision.body1 = itOther->owner;

                m_Collisions.push_back(collision);
            }
        }
    }
}

std::vector<SphereCollider> &SphereColliderSystem::GetComponents()
{
    return m_SphereColliders;
}

SphereCollider *SphereColliderSystem::GetComponent(ComponentHandle handle)
{
    if (handle > m_ComponentMapping.size())
    {
        return nullptr;
    }

    SphereCollider &component = m_SphereColliders[m_ComponentMapping[handle]];

    return &component;
}

void SphereColliderSystem::DestroyComponent(ComponentHandle handle)
{
    if (handle < m_ComponentMapping.size())
    {
        SphereCollider &component = m_SphereColliders[m_ComponentMapping[handle]];
        if (component.IsDeleted())
        {
            return;
        }
        else
        {
            component.MarkAsDeleted();
            m_FreeComponentSlots.push_back(handle);
        }
    }
}

void SphereColliderSystem::SetEntityDatabase(EntityDatabase *entityDatabase)
{
    m_EntityDatabase = entityDatabase;
}

const std::vector<Collision> &SphereColliderSystem::GetCollisions() const
{
    return m_Collisions;
}

void SphereColliderSystem::ClearCollisions()
{
    m_Collisions.clear();
}

SphereCollider::SphereCollider()
{
    radius = 1.0f;

    m_Deleted = false;
    m_Active = true;
}
