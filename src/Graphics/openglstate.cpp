#include "openglstate.hpp"
#include "graphicsutils.hpp"

void OpenGLState::Initialize()
{
    // Set all default OpenGL state

    // Cull faces
    m_EnableFaceCulling = true;

    glEnable(GL_CULL_FACE);
    GL_ERROR_CHECK();

    // Cull back faces
    m_FaceCullSide = FaceCullSide::Back;

    glCullFace(GL_BACK);
    GL_ERROR_CHECK();

    // Front face are clockwise
    m_FrontFace = FrontFaceRotation::Clockwise;

    glFrontFace(GL_CW);
    GL_ERROR_CHECK();

    // Depth is written by default
    m_DepthWriteEnabled = true;

    glEnable(GL_DEPTH_TEST);
    GL_ERROR_CHECK();

    // Write to depth by default
    m_DepthWriteEnabled = true;

    glDepthMask(GL_TRUE);
    GL_ERROR_CHECK();

    // Default depth function is less
    m_DepthTestFunction = DepthTestFunction::Less;

    glDepthFunc(GL_LESS);
    GL_ERROR_CHECK();

    // Current shader program is none
    m_CurrentShaderProgram = -1;
}

void OpenGLState::EnableFaceCulling()
{
    if (m_EnableFaceCulling)
    {
        return;
    }

    glEnable(GL_CULL_FACE);
    GL_ERROR_CHECK();

    m_EnableFaceCulling = true;
}

void OpenGLState::DisableFaceCulling()
{
    if (!m_EnableFaceCulling)
    {
        return;
    }

    glDisable(GL_CULL_FACE);
    GL_ERROR_CHECK();

    m_EnableFaceCulling = false;
}

void OpenGLState::SetFrontFaceRotation(FrontFaceRotation frontFace)
{
    EnableFaceCulling();

    if (m_FrontFace == frontFace)
    {
        return;
    }

    glFrontFace(static_cast<GLenum>(frontFace));
    GL_ERROR_CHECK();

    m_FrontFace = frontFace;
}

void OpenGLState::SetFaceCullSide(FaceCullSide faceSide)
{
    EnableFaceCulling();

    if (m_FaceCullSide == faceSide)
    {
        return;
    }

    glCullFace(static_cast<GLenum>(faceSide));
    GL_ERROR_CHECK();

    m_FaceCullSide = faceSide;
}

void OpenGLState::EnableDepthTest()
{
    if (m_DepthTestEnabled)
    {
        return;
    }

    glEnable(GL_DEPTH_TEST);
    GL_ERROR_CHECK();

    m_DepthTestEnabled = true;
}

void OpenGLState::DisableDepthTest()
{
    if (!m_DepthTestEnabled)
    {
        return;
    }

    glDisable(GL_DEPTH_TEST);
    GL_ERROR_CHECK();

    m_DepthTestEnabled = false;
}

void OpenGLState::EnableDepthWrite()
{
    if (m_DepthWriteEnabled)
    {
        return;
    }

    glDepthMask(GL_TRUE);
    GL_ERROR_CHECK();

    m_DepthWriteEnabled = true;
}

void OpenGLState::DisableDepthWrite()
{
    if (!m_DepthWriteEnabled)
    {
        return;
    }

    glDepthMask(GL_FALSE);
    GL_ERROR_CHECK();

    m_DepthWriteEnabled = false;
}

void OpenGLState::SetDepthTestFunction(DepthTestFunction depthTestFunction)
{
    if (m_DepthTestFunction == depthTestFunction)
    {
        return;
    }

    glDepthFunc(static_cast<GLenum>(depthTestFunction));
    GL_ERROR_CHECK();

    m_DepthTestFunction = depthTestFunction;
}

void OpenGLState::SetShaderProgram(const Shader &shader)
{
    int shaderId = static_cast<int>(shader.GetShaderId());

    if (m_CurrentShaderProgram == shaderId)
    {
        return;
    }

    glUseProgram(shader.GetShaderId());
    GL_ERROR_CHECK();

    m_CurrentShaderProgram = shaderId;
}
