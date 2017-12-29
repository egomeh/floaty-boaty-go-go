#pragma once

#include "collider.hpp"

#include "glm/glm.hpp"

#include <vector>

class PhysicsSystem
{
public:
    SphereColliderSystem &GetSphereColliderSystem();

private:
    SphereColliderSystem m_SphereColliderSystem;
};
