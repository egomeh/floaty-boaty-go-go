#include "texture.hpp"
#include "graphicsutils.hpp"

#include <algorithm>

GLenum CubemapAbstractFacetoOpenGL(CubemapFace face)
{
    switch (face)
    {
        case (Right):
            return GL_TEXTURE_CUBE_MAP_POSITIVE_X;
        case (Left):
            return GL_TEXTURE_CUBE_MAP_NEGATIVE_X;
        case (Top):
            return GL_TEXTURE_CUBE_MAP_POSITIVE_Y;
        case (Bottom):
            return GL_TEXTURE_CUBE_MAP_NEGATIVE_Y;
        case (Front):
            return GL_TEXTURE_CUBE_MAP_NEGATIVE_Z;
        case (Back):
            return GL_TEXTURE_CUBE_MAP_POSITIVE_Z;
        default:
            return GL_TEXTURE_CUBE_MAP_POSITIVE_X;
    }
}


Texture::Texture(std::size_t width, std::size_t height) :
    m_FilterMode(TextureFilterMode::Bilinear),
    m_WrapMode(TextureWrapMode::Clamp),
    m_Created(false)
{
    m_Width = width;
    m_Height = height;
}

GLint Texture::GetGLWrapMode(TextureWrapMode wrapMode)
{
    switch (wrapMode)
    {
        case (TextureWrapMode::Clamp):
        {
            return GL_CLAMP_TO_EDGE;
        }
        case (TextureWrapMode::Repeat):
        {
            return GL_REPEAT;
        }
        default:
        {
            return GL_CLAMP_TO_EDGE;
        }
    }

    return GL_CLAMP_TO_EDGE;
}

GLint Texture::GetGLMinFilterMode(TextureFilterMode filterMode, bool mipmap)
{
    switch (filterMode)
    {
        case (TextureFilterMode::Point):
        {
            return GL_NEAREST;
        }
        case (TextureFilterMode::Bilinear):
        {
            if (mipmap)
            {
                return GL_LINEAR_MIPMAP_NEAREST;
            }
            else
            {
                return GL_LINEAR;
            }
        }
        case (TextureFilterMode::Trilinear):
        {
            if (mipmap)
            {
                return GL_LINEAR_MIPMAP_LINEAR;
            }
            else
            {
                return GL_LINEAR;
            }
        }
    }
    return GL_NEAREST;
}

GLint Texture::GetGLMagFilterMode(TextureFilterMode filterMode)
{
    if (filterMode == TextureFilterMode::Point)
    {
        return GL_NEAREST;
    }
    else
    {
        return GL_LINEAR;
    }
}

void Texture::SetWrapMode(TextureWrapMode wrapMode)
{
    m_WrapMode = wrapMode;
}

void Texture::SetFilterMode(TextureFilterMode filterMode)
{
    m_FilterMode = filterMode;
}

void Texture::SetAnisootropicFiltering(float level)
{
    m_AnisotropicFilteringLevel = level;
}

Texture2D::Texture2D(std::size_t width, std::size_t height) :
    Texture(width, height),
    m_Mipmaps(true)
{
    m_Pixels.resize(width * height);
    std::fill(m_Pixels.begin(), m_Pixels.begin(), glm::vec4(0.));
}

Texture2D::Texture2D() : Texture2D(1, 1)
{
}

Texture2D::~Texture2D()
{
    Delete();
}

GLenum Texture2D::GetTextureType() const
{
    return GL_TEXTURE_2D;
}

void Texture2D::SetPixel(std::size_t x, std::size_t y, glm::vec4 value)
{
    if (x < m_Width && y < m_Height)
    {
        std::size_t index = y * m_Width + x;
        m_Pixels[index] = value;
    }
}

void Texture2D::SetPixels(std::vector<glm::vec4> pixels)
{
    if (pixels.size() == m_Width * m_Height)
    {
        m_Pixels = pixels;
    }
}

void Texture2D::SetDimensions(std::size_t width, std::size_t height)
{
    m_Width = width;
    m_Height = height;

    m_Pixels.resize(width * height);
    std::fill(m_Pixels.begin(), m_Pixels.begin(), glm::vec4(0.));
}

void Texture2D::GenerateMipmaps(bool mipmaps)
{
    if (m_Created)
    {
        return;
    }

    m_Mipmaps = mipmaps;
}

void Texture2D::Create()
{
    if (m_Created)
    {
        return;
    }

    glGenTextures(1, &m_TextureId);

    glBindTexture(GL_TEXTURE_2D, m_TextureId);
    GL_ERROR_CHECK();

    GLsizei textureWidth = static_cast<GLsizei>(m_Width);
    GLsizei textureHeight = static_cast<GLsizei>(m_Height);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureWidth, textureHeight, 0, GL_RGBA, GL_FLOAT, m_Pixels.data());
    GL_ERROR_CHECK();

    if (m_Mipmaps)
    {
        glGenerateMipmap(GL_TEXTURE_2D);
        GL_ERROR_CHECK();
    }

    if (m_AnisotropicFilteringLevel > 1)
    {
        if (glewIsExtensionSupported("GL_EXT_texture_filter_anisotropic"))
        {
            GL_ERROR_CHECK();

            float maxAnisotropicFilteringLevel = 0.0f;

            glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropicFilteringLevel);
            GL_ERROR_CHECK();

            float appliedAnisotropicFiltering = std::min(m_AnisotropicFilteringLevel, maxAnisotropicFilteringLevel);

            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, appliedAnisotropicFiltering);
            GL_ERROR_CHECK();
        }
    }

    m_Created = true;

    ApplySamplerParams();
}

void Texture2D::Apply() const
{
    if (!m_Created)
    {
        return;
    }

    glBindTexture(GL_TEXTURE_2D, m_TextureId);
    GL_ERROR_CHECK();

    GLsizei textureWidth = static_cast<GLsizei>(m_Width);
    GLsizei textureHeight = static_cast<GLsizei>(m_Height);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureWidth, textureHeight, 0, GL_RGBA, GL_FLOAT, m_Pixels.data());
    GL_ERROR_CHECK();

    if (m_Mipmaps)
    {
        glGenerateMipmap(GL_TEXTURE_2D);
        GL_ERROR_CHECK();
    }

    ApplySamplerParams();
}

void Texture2D::DiscardHostMemory()
{
    m_Pixels.clear();
}

void Texture2D::Delete()
{
    if (m_TextureId == 0)
    {
        return;
    }

    glDeleteTextures(1, &m_TextureId);
    m_TextureId = 0;

    DiscardHostMemory();

    m_Created = false;
}

void Texture2D::ApplySamplerParams() const
{
    glBindTexture(GL_TEXTURE_2D, m_TextureId);
    GL_ERROR_CHECK();

    const GLint wrapMode = Texture::GetGLWrapMode(m_WrapMode);
    const GLint filterMinMode = Texture::GetGLMinFilterMode(m_FilterMode, m_Mipmaps);
    const GLint filterMagMode = Texture::GetGLMagFilterMode(m_FilterMode);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
    GL_ERROR_CHECK();

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);
    GL_ERROR_CHECK();

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterMinMode);
    GL_ERROR_CHECK();

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterMagMode);
    GL_ERROR_CHECK();
}

RenderTexture::RenderTexture(std::size_t width, std::size_t height, std::size_t depthBits) : Texture(width, height),
    m_DepthBits(depthBits),
    m_NumberColorBuffers(1),
    m_SamplesPerPixel(1)
{
}

RenderTexture::~RenderTexture()
{
    Release();
}

GLenum RenderTexture::GetTextureType() const
{
    return GL_TEXTURE_2D;
}

void RenderTexture::Create()
{
    Release();

    glGenFramebuffers(1, &m_FramebufferID);
    GL_ERROR_CHECK();
    glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferID);
    GL_ERROR_CHECK();

    GLsizei width = static_cast<GLsizei>(m_Width);
    GLsizei height = static_cast<GLsizei>(m_Height);

    if (m_DepthBits == 16)
    {
        glGenRenderbuffers(1, &m_DepthBuffer);
        GL_ERROR_CHECK();
        glBindRenderbuffer(GL_RENDERBUFFER, m_DepthBuffer);
        GL_ERROR_CHECK();

        if (m_SamplesPerPixel > 1)
            glRenderbufferStorageMultisample(GL_RENDERBUFFER, m_SamplesPerPixel, GL_DEPTH_COMPONENT16, width, height);
        else
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);
        
        GL_ERROR_CHECK();
    }

    if (m_DepthBits == 24)
    {
        glGenRenderbuffers(1, &m_DepthBuffer);
        GL_ERROR_CHECK();
        glBindRenderbuffer(GL_RENDERBUFFER, m_DepthBuffer);
        GL_ERROR_CHECK();

        if (m_SamplesPerPixel > 1)
            glRenderbufferStorageMultisample(GL_RENDERBUFFER, m_SamplesPerPixel, GL_DEPTH24_STENCIL8, width, height);
        else
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);

        GL_ERROR_CHECK();
    }

    if (m_DepthBits == 32)
    {
        glGenRenderbuffers(1, &m_DepthBuffer);
        GL_ERROR_CHECK();
        glBindRenderbuffer(GL_RENDERBUFFER, m_DepthBuffer);
        GL_ERROR_CHECK();

        if (m_SamplesPerPixel > 1)
            glRenderbufferStorageMultisample(GL_RENDERBUFFER, m_SamplesPerPixel, GL_DEPTH32F_STENCIL8, width, height);
        else
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH32F_STENCIL8, width, height);

        GL_ERROR_CHECK();
    }

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_DepthBuffer);
    GL_ERROR_CHECK();

    for (int i = 0; i < m_NumberColorBuffers; ++i)
    {
        GLuint renderTarget;
        glGenTextures(1, &renderTarget);
        GL_ERROR_CHECK();
        if (m_SamplesPerPixel > 1)
            glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, renderTarget);
        else
            glBindTexture(GL_TEXTURE_2D, renderTarget);
        GL_ERROR_CHECK();

        if (m_SamplesPerPixel > 1)
            glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_SamplesPerPixel, GL_RGBA16F, width, height, GL_TRUE);
        else
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, 0);
        GL_ERROR_CHECK();

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        GL_ERROR_CHECK();
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        GL_ERROR_CHECK();
        m_RenderTargets.push_back(renderTarget);

        if (m_SamplesPerPixel > 1)
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D_MULTISAMPLE, renderTarget, 0);
        else
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, renderTarget, 0);
        GL_ERROR_CHECK();
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    GL_ERROR_CHECK();

    m_TextureId = m_RenderTargets[0];

    m_Created = true;
}

void RenderTexture::Release()
{
    if (m_DepthBuffer != 0)
    {
        glDeleteRenderbuffers(1, &m_DepthBuffer);
        GL_ERROR_CHECK();
    }

    std::remove_if(m_RenderTargets.begin(), m_RenderTargets.end(), [](GLuint id) { return id == 0; });
    if (m_RenderTargets.size() > 0)
    {
        glDeleteTextures(static_cast<GLsizei>(m_RenderTargets.size()), m_RenderTargets.data());
    }

    if (m_FramebufferID != 0)
    {
        glDeleteFramebuffers(1, &m_FramebufferID);
        GL_ERROR_CHECK();
    }

    m_DepthBuffer = 0;
    m_FramebufferID = 0;
    m_RenderTargets.clear();

    m_Created = false;
}

void RenderTexture::SetMultiSamplingLevel(std::size_t samples)
{
    m_SamplesPerPixel = samples;
}

GLuint RenderTexture::GetRenderTarget(std::size_t index)
{
    if (index < m_RenderTargets.size())
    {
        return m_RenderTargets[index];
    }
    return 0;
}

bool RenderTexture::CopyRenderTexture(RenderTexture &source)
{
    if (m_Width != source.GetWidth())
    {
        return false;
    }

    if (m_Height != source.GetHeight())
    {
        return false;
    }

    glBindFramebuffer(GL_READ_FRAMEBUFFER, source.GetFramebufferID());
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, GetFramebufferID());

    glBlitFramebuffer(0, 0, m_Width, m_Height, 0, 0, m_Width, m_Height, GL_COLOR_BUFFER_BIT, GL_LINEAR);
    GL_ERROR_CHECK();

    return true;
}

Cubemap::Cubemap() : Texture(1, 1)
{
}

Cubemap::~Cubemap()
{
    if (m_TextureId != 0)
    {
        glDeleteTextures(1, &m_TextureId);
    }
}

GLenum Cubemap::GetTextureType() const
{
    return GL_TEXTURE_CUBE_MAP;
}

void Cubemap::SetPixelDataForFace(std::vector<glm::vec4> pixelData, int width, int height, CubemapFace face)
{
    const unsigned int faceOffset = static_cast<unsigned int>(face);

    m_FacePixelData[faceOffset].pixels = pixelData;
    m_FacePixelData[faceOffset].width = width;
    m_FacePixelData[faceOffset].height = height;
}

void Cubemap::DiscardHostData()
{
    m_FacePixelData[CubemapFace::Back].pixels.clear();
    m_FacePixelData[CubemapFace::Bottom].pixels.clear();
    m_FacePixelData[CubemapFace::Front].pixels.clear();
    m_FacePixelData[CubemapFace::Left].pixels.clear();
    m_FacePixelData[CubemapFace::Right].pixels.clear();
    m_FacePixelData[CubemapFace::Top].pixels.clear();
}

void Cubemap::Create()
{
    if (m_Created)
    {
        return;
    }

    glGenTextures(1, &m_TextureId);
    GL_ERROR_CHECK();

    glBindTexture(GL_TEXTURE_CUBE_MAP, m_TextureId);
    GL_ERROR_CHECK();

    for (int i = 0; i < 6; ++i)
    {
        CubemapFace currentFaceDirection = static_cast<CubemapFace>(i);
        CubemapFaceData currentFace = m_FacePixelData[i];

        GLenum openglCubemapFace = CubemapAbstractFacetoOpenGL(currentFaceDirection);

        glTexImage2D(openglCubemapFace, 0, GL_RGBA, currentFace.width, currentFace.height, 0, GL_RGBA, GL_FLOAT, currentFace.pixels.data());
        GL_ERROR_CHECK();
    }

    // So far cubemap sampler parameters are fixed for now
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    GL_ERROR_CHECK();

    m_Created = true;
}

void Cubemap::Apply() const
{

}
