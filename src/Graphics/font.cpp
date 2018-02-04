#include "font.hpp"
#include "graphicsutils.hpp"

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

#include <algorithm>

FontTexture::FontTexture()
{
    m_FontTexture = new Texture2D(1, 1);
}

// Heavily inspired by https://github.com/0xc0dec/stb-truetype-examples/blob/master/Example1.cpp
void FontTexture::GenerateFontTexture(Font *font, unsigned int size, float fontSize, FontTextureType renderType)
{
    m_FontTexture->Delete();

    // Setup the parameters for the font texture
    m_FontTexture->SetDimensions(size, size);
    m_FontTexture->GenerateMipmaps(true);
    m_FontTexture->SetFilterMode(TextureFilterMode::Bilinear);
    m_FontTexture->SetAnisootropicFiltering(1);

    // Allocate the host pixel buffer
    std::unique_ptr<uint8_t[]> atlasData = std::make_unique<uint8_t[]>(size * size);

    uint32_t character = ' ';
    uint32_t numberOfChars = '~' - ' ';
    //stbtt_aligned_quad quad;

    std::unique_ptr<stbtt_packedchar[]> charInfo = std::make_unique<stbtt_packedchar[]>(numberOfChars);
    stbtt_pack_context context;

    // For now assume that this will not fail
    // TODO: Make check and indicate with error texture
    int packBeginSuccess = stbtt_PackBegin(&context, atlasData.get(), size, size, 0, 1, nullptr);

    stbtt_PackSetOversampling(&context, 2, 2);

    // For now assume  that this will not fail
    // TODO: Make check and indicate with error texture
    const unsigned char *fontData = font->GetRawFontData().data();
    uint32_t firstChar = ' ';
    stbtt_PackFontRange(&context, fontData, 0, fontSize, firstChar, numberOfChars, charInfo.get());

    stbtt_PackEnd(&context);

    std::vector<glm::vec4> pixels = std::vector<glm::vec4>(size * size);

    unsigned int iterator = 0;
    for (glm::vec4 &pixel : pixels)
    {
        const float pixelValue = static_cast<float>(atlasData.get()[iterator] / 255.f);
        pixel = glm::vec4(pixelValue, pixelValue, pixelValue, 1.f);
        ++iterator;
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
