#pragma once

#include "graphicsutils.hpp"
#include "glm/glm.hpp"
#include "graphics.hpp"

enum TextureFilterMode
{
    Point,
    Bilinear,
    Trilinear
};

enum TextureWrapMode
{
    Clamp,
    Repeat,
};

enum TextureFormat
{
    RGBA32,
    RGBA32F
};

enum CubemapFace
{
    Right = 0,
    Left,
    Top,
    Bottom,
    Back,
    Front
};

GLenum CubemapAbstractFacetoOpenGL(CubemapFace face);

class Texture
{
public:
    Texture(std::size_t width, std::size_t height);

    GLuint GetTextureID() const { return m_TextureId; };
    virtual GLenum GetTextureType() const = 0;

    static GLint GetGLWrapMode(TextureWrapMode wrapMode);
    static GLint GetGLMinFilterMode(TextureFilterMode filterMode, bool mipmap);
    static GLint GetGLMagFilterMode(TextureFilterMode filterMode);

    std::size_t GetWidth() const { return m_Width; };
    std::size_t GetHeight() const { return m_Height; };

    virtual void SetWrapMode(TextureWrapMode wrapMode);
    virtual void SetFilterMode(TextureFilterMode filterMode);
    virtual void SetAnisootropicFiltering(float level);
    virtual void Create() = 0;
protected:

    std::size_t m_Width, m_Height;
    float m_AnisotropicFilteringLevel;
    GLuint m_TextureId;
    TextureFilterMode m_FilterMode;
    TextureWrapMode m_WrapMode;
    TextureFormat m_TextureFormat;
    bool m_Created;
};

class Texture2D : public Texture
{
public:
    Texture2D(std::size_t width, std::size_t height);
    Texture2D();
    ~Texture2D();

    GLenum GetTextureType() const;

    void SetPixel(std::size_t x, std::size_t y, glm::vec4 value);
    void SetPixels(std::vector<glm::vec4> pixels);
    void SetDimensions(std::size_t width, std::size_t height);

    void GenerateMipmaps(bool mipmaps);

    void Create() override;
    void Apply() const;

    void DiscardHostMemory();
    void Delete();

private:
    void ApplySamplerParams() const;

private:
    std::vector<glm::vec4> m_Pixels;
    bool m_Mipmaps;
};

class RenderTexture : public Texture
{
public:
    RenderTexture(std::size_t width, std::size_t height, std::size_t depthBits);
    ~RenderTexture();

    GLenum GetTextureType() const;

    void Create() override;
    void Release();

    void SetMultiSamplingLevel(std::size_t samples);

    GLuint GetFramebufferID() { return m_FramebufferID; };

    GLuint GetRenderTarget(std::size_t index);

    bool CopyRenderTexture(RenderTexture &source);

private:
    GLuint m_FramebufferID;
    std::size_t m_NumberColorBuffers;
    std::size_t m_DepthBits;
    GLuint m_DepthBuffer;
    std::vector<GLuint> m_RenderTargets;
    std::size_t m_SamplesPerPixel;
};

struct CubemapFaceData
{
public:
    std::vector<glm::vec4> pixels;
    int width;
    int height;
};

class Cubemap : public Texture
{
public:
    Cubemap();
    ~Cubemap();

    GLenum GetTextureType() const override;

    void SetPixelDataForFace(std::vector<glm::vec4> pixelData, int width, int height, CubemapFace face);
    void DiscardHostData();

    void Create() override;
    void Apply() const;

private:
    CubemapFaceData m_FacePixelData[6];
};
