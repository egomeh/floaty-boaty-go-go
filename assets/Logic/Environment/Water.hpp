#pragma once

#include "logic.hpp"

#include "Graphics/mesh.hpp"

class Water : public LogicComponent
{
public:
    Water();

    void Update() override;
    void Start() override;

    template<typename SerializerType>
    void Deserialize(SerializerType serializer)
    {
        DESERIALIZE(m_MaterialName, serializer);
    }

private:
    std::string m_MaterialName;

    Mesh m_WaterPlane;
};


