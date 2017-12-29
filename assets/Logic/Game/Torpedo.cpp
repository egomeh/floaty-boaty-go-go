#include "Torpedo.hpp"

#include "util.hpp"
#include "transform.hpp"

#include "ShipMovement.hpp"

#include <algorithm>

Torpedo::Torpedo()
{
}

void Torpedo::Update()
{
    m_Timer += GetFrameContext().deltaTime / m_LifeTime;

    // Lower speed
    m_Speed = std::max(5.0f, m_Speed - GetFrameContext().deltaTime);

    // Check for life time
    if (m_Timer > 1.0f)
    {
        m_EntityDatabase->MarkEntityForDestruction(owner);

        return;
    }

    Transform *transform = GetComponent<Transform>();

    glm::quat rotation = transform->GetLocalRotation();

    glm::vec3 forwards = transform->GetLocalForwards();

    glm::vec3 position = transform->GetLocalPosition();

    // Resolve movement
    position += forwards * m_Speed * GetFrameContext().deltaTime;

    transform->SetLocalPosition(position);

    // Resolve rotation
    glm::quat currentRotation = transform->GetLocalRotation();

    const float rotationSign = static_cast<float>(sign(m_Rotation));
    const float rotationAmount = .05f * m_Speed * GetFrameContext().deltaTime;

    const float totalRotation = std::abs(m_Rotation);

    const float rotationChunk = std::min(rotationAmount, totalRotation);

    const float addedRotation = rotationChunk * rotationSign;

    m_Rotation -= addedRotation;

    currentRotation *= glm::quat(glm::vec3(0, addedRotation, 0));

    transform->SetLocalRotation(currentRotation);
}

void Torpedo::OnCollision(EntityHandle other)
{
    ShipMovement *targetShip = m_EntityDatabase->GetComponent<ShipMovement>(other);

    if (targetShip != nullptr)
    {
        m_EntityDatabase->MarkEntityForDestruction(targetShip->owner);
        m_EntityDatabase->MarkEntityForDestruction(owner);
    }
}

void Torpedo::Start()
{
    m_Timer = 0.0f;
}

void Torpedo::SetRotation(float rotation)
{
    m_Rotation = rotation;
}
