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

    void SetRawFontData(const std::vector<uint8_t> fontData);
    const std::vector<uint8_t> &GetRawFontData() const;

private:
    std::vector<uint8_t> m_RawFontData;
};

class FontTexture
{
public:
    FontTexture();
    
    void GenerateFontTexture(Font *fontBuffer, unsigned int size, FontTextureType renderType);

private:
    Texture2D *m_FontTexture;
};
