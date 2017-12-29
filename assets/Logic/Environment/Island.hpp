#pragma once

#include "logic.hpp"
#include "util.hpp"

#include "Graphics/mesh.hpp"
#include "Graphics/material.hpp"

#include "delaunay.h"

class Island : public LogicComponent
{
public:
    Island()
    {
    }

    void Update() override;
    void Start() override;

    template<typename SerializerType>
    void Deserialize(SerializerType serializer)
    {
        DESERIALIZE(material, serializer);
    }

private:
    std::string material;
    Material *m_Material;
    Mesh m_IslandMesh;
};
