#pragma once

#include <windows.h>
#include <sstream>

#include "logic.hpp"
#include "util.hpp"

class FreeFlight : public LogicComponent
{
public:
    FreeFlight()
    {
        m_Pitch = 0.0f;
        m_Yaw = 0.0f;

        m_Velocity = glm::vec3(0);

        m_AllowFreeMovement = true;
    }

    void SetFreeeMovement(bool freeMovement);

    void Update() override;

    template<typename SerializerType>
    void Deserialize(SerializerType serializer)
    {
        DESERIALIZE(m_Speed, serializer);
    }

private:
    float m_Speed;
    float m_Pitch;
    float m_Yaw;

    bool m_AllowFreeMovement;

    glm::vec3 m_Velocity;
};
