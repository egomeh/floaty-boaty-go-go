#pragma once

#include "logic.hpp"
#include "util.hpp"

#include "Graphics/material.hpp"
#include "Graphics/mesh.hpp"

class OverlayImage : public LogicComponent
{
public:
    OverlayImage()
    {
    }

    void Update() override;
    void Start() override;

    template<typename SerializerType>
    void Deserialize(SerializerType serializer)
    {
        DESERIALIZE(m_ImageName, serializer);
    }

private:
    std::string m_ImageName;
    
    std::shared_ptr<Material> m_Material;
    Mesh m_Mesh;
};
