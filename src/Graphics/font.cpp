#include "font.hpp"
#include "graphicsutils.hpp"

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

#include <algorithm>

FontTexture::FontTexture()
{
    m_FontTexture = new Texture2D(1, 1);
}

void FontTexture::GenerateFontTexture(Font *fontBuffer, unsigned int size, FontTextureType renderType)
{
    m_FontTexture->Delete();

    // Setup the parameters for the font texture
    m_FontTexture->SetDimensions(size, size);
    m_FontTexture->GenerateMipmaps(true);
    m_FontTexture->SetFilterMode(TextureFilterMode::Bilinear);
    m_FontTexture->SetAnisootropicFiltering(1);
    
    // Allocate the host pixel buffer
    std::vector<glm::vec4> pixels = std::vector<glm::vec4>(size * size);

    for (glm::vec4 &pixel : pixels)
    {
        pixel = glm::vec4(.75f, .5f, .5f, 1.f);
    }

    // Set pixels, generate the texture and discard data on
    // CPU memory
    m_FontTexture->SetPixels(pixels);
    m_FontTexture->Apply();
    m_FontTexture->Create();
    m_FontTexture->DiscardHostMemory();
}

Texture2D *FontTexture::GetTexture() const
{
    return m_FontTexture;
}

Font::Font()
{
}

void Font::SetRawFontData(const std::vector<uint8_t> fontData)
{
    m_RawFontData = fontData;
}

const std::vector<uint8_t>& Font::GetRawFontData() const
{
    return m_RawFontData;
}
