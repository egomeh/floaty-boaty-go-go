#include "FreeFlight.hpp"

#include "input.hpp"

#include <algorithm>

void FreeFlight::SetFreeeMovement(bool freeMovement)
{
    m_AllowFreeMovement = freeMovement;
}

void FreeFlight::Update()
{
    Transform *transform = GetComponent<Transform>();

    glm::vec3 forwards = transform->GetLocalForwards();
    glm::vec3 right = transform->GetLocalRight();

    m_Pitch += GetFrameContext().deltaMouse.y * 0.5f;
    m_Yaw -= GetFrameContext().deltaMouse.x * 0.5f;

    m_Pitch = std::max(std::min(m_Pitch, 80.0f), -80.0f);

    transform->SetLocalEulerAngles(glm::vec3(m_Pitch, m_Yaw, 0.0f));

    if (!m_AllowFreeMovement)
    {
        return;
    }

    glm::vec3 position = transform->GetLocalPosition();

    float move = 0.f;
    float strafe = 0.f;

    move = Input::KeyValue('w') - Input::KeyValue('s');

    strafe = Input::KeyValue('a') - Input::KeyValue('d');

    if (move != 0.0f || strafe != 0.0f)
    {
        glm::vec3 acceleration = (forwards * move + right * strafe) * m_Speed;

        m_Velocity += (acceleration) * GetFrameContext().deltaTime;

        position = position + acceleration * GetFrameContext().deltaTime;

        transform->SetLocalPosition(position);
    }
}
