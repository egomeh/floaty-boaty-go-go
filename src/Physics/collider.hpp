#pragma once

#include "ecs.hpp"
#include "collision.hpp"
#include "serialization/serialization.hpp"
#include "transform.hpp"

class SphereCollider : public ComponentBase
{
public:
    SphereCollider();

    template<typename SerializationType>
    void Deserialize(SerializationType serializer)
    {
        DESERIALIZE(radius, serializer);
    }

    float radius;
};

class SphereColliderSystem : public ComponentSystem<SphereCollider>
{
public:
    bool Create(ComponentHandle &handle);

    void Update();

    std::vector<SphereCollider> &GetComponents();
    SphereCollider *GetComponent(ComponentHandle handle) override;

    void DestroyComponent(ComponentHandle handle) override;

    void SetEntityDatabase(EntityDatabase *entityDatabase);

    const std::vector<Collision> &GetCollisions() const;
    void ClearCollisions();

private:
    std::vector<SphereCollider> m_SphereColliders;

    std::vector<std::size_t> m_FreeComponentSlots;
    std::vector<int> m_ComponentMapping;

    EntityDatabase *m_EntityDatabase;

    std::vector<Collision> m_Collisions;
};

