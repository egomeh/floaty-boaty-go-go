#include "font.hpp"
#include "graphicsutils.hpp"

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

#include <algorithm>

FontTexture::FontTexture()
{
}

void FontTexture::GenerateFontTexture(char *fontBuffer, unsigned int size, FontTextureType renderType)
{
}

Font::Font()
{
}

void Font::SetRawFontData(const std::vector<char> fontData)
{
    m_RawFontData = fontData;
}

const std::vector<char>& Font::GetRawFontData() const
{
    return m_RawFontData;
}
