#pragma once

#include "assetdatabase.hpp"
#include "ecs.hpp"
#include "physics/physics.hpp"

class SphereColliderLoader : public ComponentLoader<SphereCollider>
{
public:
    TypedComponentHandle LoadComponent(const nlohmann::json &serializedComponent) override;
    TypedComponentHandle LoadComponent(ComponentTypeId typeId) override;
    ComponentBase *GetAbstractComponent(ComponentHandle handle);

    void SetSphereColliderSystem(SphereColliderSystem *system);
private:
    SphereColliderSystem *m_SphereColliderSystem;
};
