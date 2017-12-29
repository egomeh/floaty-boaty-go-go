#pragma once

#include <string>
#include <map>

#include "glm/glm.hpp"

enum LightType
{
    Point,
    Directional,
    Spot
};

namespace LightTypeKeywords
{
    static const std::string Point("point");
    static const std::string Directional("directional");
    static const std::string Spot("spot");
}

const std::map<std::string, LightType> clearTypeKeywordMap =
{
    {LightTypeKeywords::Point, Point},
    {LightTypeKeywords::Directional, Directional},
    {LightTypeKeywords::Spot, Spot}
};


class Light
{
public:
    
    template<typename SerializrType>
    void Deserialize(SerializrType serializer)
    {

    }

private:

    // General light info
    float m_Intensity;
    glm::vec3 m_Color;

    // Spotlight info
    float m_Angle;

    // Point and spot light
    float m_Range;

    LightType m_LightType;
};
