#pragma once

#include "logic.hpp"
#include "scene.hpp"

#include <windows.h>
#include "util.hpp"
#include <sstream>

class AIControls : public LogicComponent
{
public:
    AIControls()
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
