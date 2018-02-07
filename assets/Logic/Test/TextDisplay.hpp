#pragma once

#include "logic.hpp"
#include "util.hpp"

#include "Graphics/mesh.hpp"
#include "Graphics/font.hpp"
#include "Graphics/material.hpp"

class TextDisplay : public LogicComponent
{
public:
    TextDisplay()
    {
    }

    void Update() override;
    void Start() override;

    template<typename SerializerType>
    void Deserialize(SerializerType serializer)
    {
        DESERIALIZE(m_DisplayText, serializer);
    }

private:
    std::string m_DisplayText;
    Mesh m_Mesh;
    Material m_Material;
};
