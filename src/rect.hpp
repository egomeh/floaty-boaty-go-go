#pragma once

#include "glm/glm.hpp"

class Rect
{
public:
    Rect();
    Rect(float x, float y, float width, float height);

    float GetX() const noexcept;
    float GetY() const noexcept;
    float GetWidth() const noexcept;
    float GetHeight() const noexcept;

    glm::vec2 GetUpperLeftCorner() const noexcept;
    glm::vec2 GetUpperRightCorner() const noexcept;
    glm::vec2 GetLowerLeftCorner() const noexcept;
    glm::vec2 GetLowerRightCorner() const noexcept;

private:
    glm::vec2 m_UpperLeftCorner;
    glm::vec2 m_LowerRightCorner;
};
