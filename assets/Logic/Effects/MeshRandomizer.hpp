#pragma once

#include "logic.hpp"

#include "Graphics/mesh.hpp"

class MeshRandomizer : public LogicComponent
{
public:
    MeshRandomizer();

    void Update() override;
    void Start() override;

    template<typename SerializerType>
    void Deserialize(SerializerType serializer)
    {
        DESERIALIZE(m_MesnNames[0], serializer);
        DESERIALIZE(m_MesnNames[1], serializer);
        DESERIALIZE(m_MesnNames[2], serializer);
        DESERIALIZE(m_MaterialName, serializer);
    }

private:
    std::string m_MesnNames[3];
    std::string m_MaterialName;
};


