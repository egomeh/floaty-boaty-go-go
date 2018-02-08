#pragma once

#include "logic.hpp"

class Rotator : public LogicComponent
{
public:
    Rotator();

    void Update() override;
    void Start() override;

    template<typename SerializerType>
    void Deserialize(SerializerType serializer)
    {
        DESERIALIZE(speed, serializer);
    }

private:
    float rotation;
    float speed;
};


