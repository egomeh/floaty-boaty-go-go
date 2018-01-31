#pragma once

#include "graphicsutils.hpp"
#include "texture.hpp"
#include "graphics.hpp"

enum class FontTextureType
{
    Raster,
    SignedDistanceField, // Not implemented
    MultiChannelSignedDistanceField, // Not implemented
};

class Font
{
public:
    Font();

    void SetRawFontData(const std::vector<char> fontData);
    const std::vector<char> &GetRawFontData() const;

private:
    std::vector<char> m_RawFontData;
};

class FontTexture
{
public:
    FontTexture();
    
    void GenerateFontTexture(char *fontBuffer, unsigned int size, FontTextureType renderType);

private:
    Texture2D *m_FontTexture;
};
