#include "rect.hpp"

Rect::Rect()
{
    m_UpperLeftCorner = glm::vec2(0.f, 0.f);
    m_LowerRightCorner = glm::vec2(0.f, 0.f);
}

Rect::Rect(float x, float y, float width, float height) : m_UpperLeftCorner(x, y), m_LowerRightCorner(x + width, y + height)
{
}

float Rect::GetX() const noexcept
{
    return m_UpperLeftCorner.x;
}

float Rect::GetY() const noexcept
{
    return m_UpperLeftCorner.y;
}

float Rect::GetWidth() const noexcept
{
    return m_LowerRightCorner.x - m_UpperLeftCorner.x;
}

float Rect::GetHeight() const noexcept
{
    return m_LowerRightCorner.y - m_UpperLeftCorner.y;
}

glm::vec2 Rect::GetUpperLeftCorner() const noexcept
{
    return m_UpperLeftCorner;
}

glm::vec2 Rect::GetUpperRightCorner() const noexcept
{
    return glm::vec2(m_LowerRightCorner.x, m_UpperLeftCorner.y);
}

glm::vec2 Rect::GetLowerLeftCorner() const noexcept
{
    return glm::vec2(m_UpperLeftCorner.x, m_LowerRightCorner.y);
}

glm::vec2 Rect::GetLowerRightCorner() const noexcept
{
    return m_LowerRightCorner;
}
