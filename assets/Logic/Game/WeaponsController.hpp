#pragma once

#include "logic.hpp"
#include "util.hpp"

class WeaponsController : public LogicComponent
{
public:
    WeaponsController()
    {
    }

    void Update() override;
    void Start() override;

    template<typename SerializerType>
    void Deserialize(SerializerType serializer)
    {
    }

private:
};
