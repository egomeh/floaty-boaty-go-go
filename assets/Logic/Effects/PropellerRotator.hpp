#pragma once

#include "logic.hpp"

class PropellerRotator : public LogicComponent
{
public:
    PropellerRotator();

    void Update() override;

    template<typename SerializerType>
    void Deserialize(SerializerType serializer)
    {
        DESERIALIZE(m_EulerRotation, serializer);
    }

private:
    glm::vec3 m_EulerRotation;
};


