#pragma once

#include "logic.hpp"
#include "util.hpp"
#include "Graphics/material.hpp"

class FXAA : public LogicComponent
{
public:

    void Update() override;
    void Start() override;
    void OnRenderImage(Texture *source, RenderTexture *destination) override;

    template<typename SerializerType>
    void Deserialize(SerializerType serializer)
    {
    }

private:
    Material *m_Material;
};
