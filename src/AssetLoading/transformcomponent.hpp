#pragma once

#include "assetdatabase.hpp"
#include "ecs.hpp"
#include "transform.hpp"

class TransformLoader : public ComponentLoader<Transform>
{
public:
    TypedComponentHandle LoadComponent(const nlohmann::json &serializedComponent) override;
    TypedComponentHandle LoadComponent(ComponentTypeId typeId) override;
    ComponentBase *GetAbstractComponent(ComponentHandle handle);

    void SetTransformSystem(TransformSystem *transformSystem);
private:
    TransformSystem *m_TransformSystem;
};
