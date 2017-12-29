#pragma once

#include "logic.hpp"

#include <random>

enum HeightLayer
{
    Lowest = 0,
    Mid = 1,
    Highest = 2
};

float HeightLayerToWorldHeight(HeightLayer layer);

class ShipMovement : public LogicComponent
{
public:
    ShipMovement();

    void Update() override;
    void Start() override;

    void SetTargetHeight(HeightLayer layer);
    HeightLayer GetTargetHeight();

    void SetThrottle(float throttle);
    void SetTurn(float sharpness);

    template<typename SerializerType>
    void Deserialize(SerializerType serializer)
    {
        DESERIALIZE(m_Speed, serializer);
        DESERIALIZE(m_Drag, serializer);

        m_IsTurning = false;
        m_HeightLayer = Mid;
        m_Throttle = 0.0f;
        m_Turn = 0.0f;
    }

private:
    float m_Speed;
    float m_Velocity;

    bool m_IsTurning;

    float m_Drag;

    float m_Throttle;
    float m_Turn;

    HeightLayer m_HeightLayer;
};


