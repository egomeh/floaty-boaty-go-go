#pragma once

#include "graphicsutils.hpp"
#include "mesh.hpp"
#include "texture.hpp"
#include "graphics.hpp"
#include "rect.hpp"

#include "stb_truetype.h"

enum class FontTextureType
{
    Raster,
    SignedDistanceField, // Not implemented
    MultiChannelSignedDistanceField, // Not implemented
};

enum class TextAlign
{
    Left,
    Center,
    Right
};

class GlyphInfo
{
public:
    GlyphInfo();

    void SetPosition(Rect position) noexcept;
    Rect GetPosiition() const noexcept;

    void SetTexcoords(Rect position) noexcept;
    Rect GetTexcoords() const noexcept;

    void SetOffset(glm::vec2 offset) noexcept;
    glm::vec2 GetOffset() const noexcept;

private:
    glm::vec2 m_Offset;
    Rect m_Position;
    Rect m_Texcoords;
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
    
    void GenerateFontTexture(Font *font, unsigned int textureSize, float fontSize, FontTextureType renderType);
    Texture2D *GetTexture() const;

    GlyphInfo GetGlyphInfo(uint32_t character, float offsetX, float offsetY) const;

    void GenerateTextMesh(Mesh &mesh, const std::string &text, TextAlign alignment);

private:
    Texture2D *m_FontTexture;
    uint32_t m_FirstChar;
    std::unique_ptr<stbtt_packedchar[]> m_PackedCharInfo;
};
