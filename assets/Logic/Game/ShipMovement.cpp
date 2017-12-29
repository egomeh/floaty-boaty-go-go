#include "ShipMovement.hpp"

#include "transform.hpp"

#include "glm/gtc/random.hpp"
#include "glm/gtx/vector_angle.hpp"
#include "glm/glm.hpp"

ShipMovement::ShipMovement()
{
    m_Drag = 1.0;
}

void ShipMovement::Update()
{
    Transform *transform = GetComponent<Transform>();

    glm::vec3 forward = transform->GetLocalForwards();

    glm::vec3 position = transform->GetLocalPosition();

    m_Velocity += (m_Throttle * m_Speed - m_Drag * m_Velocity) * GetFrameContext().deltaTime;

    position += forward * GetFrameContext().deltaTime * m_Velocity;

    const float targetHeight = HeightLayerToWorldHeight(m_HeightLayer);
    const float ascendSpeed = .5f * GetFrameContext().deltaTime;

    const float heightDiff = targetHeight - position.y;

    if (std::abs(heightDiff) < ascendSpeed)
    {
        position.y = targetHeight;
    }
    else
    {
        float sign = 1.f;

        if (heightDiff < 0.f)
        {
            sign = -1.f;
        }

        position.y += sign * ascendSpeed;
    }

    glm::quat rotation = transform->GetLocalRotation();

    rotation *= glm::quat(glm::vec3(0, m_Turn * GetFrameContext().deltaTime, 0));

    transform->SetLocalRotation(rotation);
    transform->SetLocalPosition(position);
}

void ShipMovement::Start()
{
    m_Velocity = 0.f;
}

void ShipMovement::OnDestroy()
{
    OnShipDestroyed.Invoke(owner);
}

void ShipMovement::SetTargetHeight(HeightLayer layer)
{
    if (layer < Lowest)
    {
        m_HeightLayer = Lowest;
        return;
    }

    if (layer > Highest)
    {
        m_HeightLayer = Highest;
        return;
    }

    m_HeightLayer = layer;
}

HeightLayer ShipMovement::GetTargetHeight()
{
    return m_HeightLayer;
}

void ShipMovement::SetThrottle(float throttle)
{
    m_Throttle = std::min(1.0f, std::max(0.0f, throttle));
}

void ShipMovement::SetTurn(float sharpness)
{
    m_Turn = std::min(1.0f, std::max(-1.0f, sharpness));
}

float HeightLayerToWorldHeight(HeightLayer layer)
{
    switch (layer)
    {
        case (Lowest):
        {
            return 4.0f;

            break;
        }
        case (Mid):
        {
            return 7.0f;

            break;
        }
        case(Highest):
        {
            return 10.0f;

            break;
        }
        default:
        {
            return 6.0f;
        }
    }
}
