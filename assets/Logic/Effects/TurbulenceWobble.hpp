#pragma once

#include "logic.hpp"

#include <random>

class TurbulenceWobble : public LogicComponent
{
public:
    TurbulenceWobble();

    void Update() override;

    template<typename SerializerType>
    void Deserialize(SerializerType serializer)
    {
        DESERIALIZE(m_WobbleTime, serializer);

        PickRandomDisplacement();
    }

private:

    void PickRandomDisplacement();

    glm::vec3 m_OldWobbleAngle;
    glm::vec3 m_OldDisplacement;

    glm::vec3 m_WobbleAngle;
    glm::vec3 m_WobbleDisplacement;

    float m_WobbleProgress;
    float m_WobbleTime;

    std::mt19937 rng;
};


