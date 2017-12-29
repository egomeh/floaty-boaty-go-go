#pragma once

#include "logic.hpp"

class Torpedo : public LogicComponent
{
public:
    Torpedo();

    void Update() override;
    void Start() override;
    void OnCollision(EntityHandle other) override;

    void SetRotation(float rotation);

    template<typename SerializerType>
    void Deserialize(SerializerType serializer)
    {
        DESERIALIZE(m_LifeTime, serializer);
        DESERIALIZE(m_Speed, serializer);

        if (m_LifeTime < 0.1f)
        {
            m_LifeTime = 1.0f;
        }
    }

private:
    float m_Rotation;

    float m_Timer;
    float m_LifeTime;
    float m_Speed;
};


