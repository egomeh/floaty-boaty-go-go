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
void FontTexture::GenerateFontTexture(Font *font, unsigned int textureSize, float fontSize, FontTextureType renderType)
{
    m_FontTexture->Delete();

    // Setup the parameters for the font texture
    m_FontTexture->SetDimensions(textureSize, textureSize);
    m_FontTexture->GenerateMipmaps(true);
    m_FontTexture->SetFilterMode(TextureFilterMode::Bilinear);
    m_FontTexture->SetAnisootropicFiltering(1);

    // Allocate the host pixel buffer
    std::unique_ptr<uint8_t[]> atlasData = std::make_unique<uint8_t[]>(textureSize * textureSize);

    uint32_t character = ' ';
    uint32_t numberOfChars = '~' - ' ';
    //stbtt_aligned_quad quad;

    m_PackedCharInfo = std::make_unique<stbtt_packedchar[]>(numberOfChars);
    stbtt_pack_context context;

    // For now assume that this will not fail
    // TODO: Make check and indicate with error texture
    int packBeginSuccess = stbtt_PackBegin(&context, atlasData.get(), textureSize, textureSize, 0, 1, nullptr);

    stbtt_PackSetOversampling(&context, 2, 2);

    // For now assume  that this will not fail
    // TODO: Make check and indicate with error texture
    const unsigned char *fontData = font->GetRawFontData().data();
    m_FirstChar = ' ';
    stbtt_PackFontRange(&context, fontData, 0, fontSize, m_FirstChar, numberOfChars, m_PackedCharInfo.get());

    stbtt_PackEnd(&context);

    std::vector<glm::vec4> pixels = std::vector<glm::vec4>(textureSize * textureSize);

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

// Heavily inspired by https://github.com/0xc0dec/stb-truetype-examples/blob/master/Example1.cpp
GlyphInfo FontTexture::GetGlyphInfo(uint32_t character, float offsetX, float offsetY) const
{
    stbtt_aligned_quad quad;

    const int textureSize = static_cast<int>(m_FontTexture->GetWidth());

    stbtt_GetPackedQuad(m_PackedCharInfo.get(), textureSize, textureSize, character - m_FirstChar, &offsetX, &offsetY, &quad, 1);

    float xmin = quad.x0;
    float xmax = quad.x1;
    float ymin = -quad.y0;
    float ymax = -quad.y1;

    GlyphInfo glyphInfo;

    Rect position(xmin, ymin, xmax - xmin, ymax - ymin);
    Rect texcoords(quad.s0, quad.t0, quad.s1 - quad.s0, quad.t1 - quad.t0);

    glyphInfo.SetOffset(glm::vec2(offsetX, offsetY));
    glyphInfo.SetPosition(position);
    glyphInfo.SetTexcoords(texcoords);

    return glyphInfo;
}

void FontTexture::GenerateTextMesh(Mesh &mesh, const std::string &text)
{
    // Clear the mesh in case it was already populated
    mesh.ClearIndecies();
    mesh.DeleteDeviceBuffers();

    // Make the buffers for device data
    std::shared_ptr<BasicMeshAttribute<glm::vec3>> positionAttribute = std::make_shared<BasicMeshAttribute<glm::vec3>>(Float, 3);
    std::shared_ptr<BasicMeshAttribute<glm::vec2>> texcoordAttribute = std::make_shared<BasicMeshAttribute<glm::vec2>>(Float, 2);
    std::vector<unsigned int> indicies;

    float offsetX = 0.f;
    float offsetY = 0.f;

    unsigned int index = 0;

    for (char letter : text)
    {
        uint32_t character = static_cast<uint32_t>(letter);

        GlyphInfo glyphInfo = GetGlyphInfo(character, offsetX, offsetY);

        glm::vec3 upperLeftCorner = glm::vec3(glyphInfo.GetPosiition().GetUpperLeftCorner(), 0.f);
        glm::vec3 upperRightCorner = glm::vec3(glyphInfo.GetPosiition().GetUpperRightCorner(), 0.f);
        glm::vec3 lowerLeftCorner = glm::vec3(glyphInfo.GetPosiition().GetLowerLeftCorner(), 0.f);
        glm::vec3 lowerRightCorner = glm::vec3(glyphInfo.GetPosiition().GetLowerRightCorner(), 0.f);

        glm::vec2 upperLeftTexcoord = glyphInfo.GetTexcoords().GetUpperLeftCorner();
        glm::vec2 upperRightTexcoord = glyphInfo.GetTexcoords().GetUpperRightCorner();
        glm::vec2 lowerLeftTexcoord = glyphInfo.GetTexcoords().GetLowerLeftCorner();
        glm::vec2 lowerRightTexcoord = glyphInfo.GetTexcoords().GetLowerRightCorner();

        positionAttribute->AddData(upperLeftCorner);
        positionAttribute->AddData(upperRightCorner);
        positionAttribute->AddData(lowerLeftCorner);
        positionAttribute->AddData(lowerRightCorner);

        texcoordAttribute->AddData(upperLeftTexcoord);
        texcoordAttribute->AddData(upperRightTexcoord);
        texcoordAttribute->AddData(lowerLeftTexcoord);
        texcoordAttribute->AddData(lowerRightTexcoord);

        indicies.push_back(index);
        indicies.push_back(index + 1);
        indicies.push_back(index + 2);

        indicies.push_back(index + 1);
        indicies.push_back(index + 3);
        indicies.push_back(index + 2);

        index += 4;

        offsetX = glyphInfo.GetOffset().x;
        offsetY = glyphInfo.GetOffset().y;
    }

    mesh.SetAttribute(0, positionAttribute);
    mesh.SetAttribute(1, texcoordAttribute);
    mesh.SetIndecies(indicies);

    mesh.GenerateDeviceBuffers();
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

GlyphInfo::GlyphInfo()
{
}

void GlyphInfo::SetPosition(Rect position) noexcept
{
    m_Position = position;
}

Rect GlyphInfo::GetPosiition() const noexcept
{
    return m_Position;
}

void GlyphInfo::SetTexcoords(Rect texcoords) noexcept
{
    m_Texcoords = texcoords;
}

Rect GlyphInfo::GetTexcoords() const noexcept
{
    return m_Texcoords;
}

void GlyphInfo::SetOffset(glm::vec2 offset) noexcept
{
    m_Offset = offset;
}

glm::vec2 GlyphInfo::GetOffset() const noexcept
{
    return m_Offset;
}
