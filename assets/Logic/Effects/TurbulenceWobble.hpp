#pragma once

#include "logic.hpp"

#include <random>

class TurbulenceWobble : public LogicComponent
{
public:
    TurbulenceWobble();

    void Update() override;
    void Start() override;

    template<typename SerializerType>
    void Deserialize(SerializerType serializer)
    {
        DESERIALIZE(m_WobbleTime, serializer);
    }

private:

    glm::vec3 m_OldWobbleAngle;
    glm::vec3 m_OldDisplacement;

    glm::vec3 m_WobbleAngle;
    glm::vec3 m_WobbleDisplacement;

    float m_WobbleProgress;
    float m_WobbleTime;

    // A random number used to mix int the noise pattern when wobbling.
    float m_RandomNumberFromStart;

    std::mt19937 rng;
};


