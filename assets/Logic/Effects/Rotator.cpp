#include "rotator.hpp"

#include "util.hpp"
#include "transform.hpp"

Rotator::Rotator()
{
    rotation = 0.f;
    speed = 1.0f;
}

void Rotator::Update()
{
    Transform *transform = GetComponent<Transform>();

    float deltaTime = GetFrameContext().deltaTime;

    rotation += speed * deltaTime;

    transform->SetLocalEulerAngles(glm::vec3(0, rotation, 0));
}
