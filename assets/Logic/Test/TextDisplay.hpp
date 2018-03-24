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
        m_TextAlign = TextAlign::Left;
    }

    void Update() override;
    void Start() override;

    template<typename SerializerType>
    void Deserialize(SerializerType serializer)
    {
        DESERIALIZE(m_DisplayText, serializer);

        std::string textAlign;
        DESERIALIZE(textAlign, serializer);

        if (textAlign.compare("left") == 0)
        {
            m_TextAlign = TextAlign::Left;
        }
        else if (textAlign.compare("center") == 0)
        {
            m_TextAlign = TextAlign::Center;
        }
        else if (textAlign.compare("right") == 0)
        {
            m_TextAlign = TextAlign::Right;
        }
    }

private:
    TextAlign m_TextAlign;
    std::string m_DisplayText;
    Mesh m_Mesh;
    Material m_Material;
};
