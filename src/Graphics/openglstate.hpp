#pragma once

#include "shader.hpp"

enum class FaceCullSide
{
    Front = GL_FRONT,
    Back = GL_BACK
};

enum class FrontFaceRotation
{
    Clockwise = GL_CW,
    Counterclockwise = GL_CCW
};

enum class DepthTestFunction
{
    Never = GL_NEVER,
    Less = GL_LESS,
    Equal = GL_EQUAL,
    LessOrEqual = GL_LEQUAL,
    Greater = GL_GREATER,
    NotEqual = GL_NOTEQUAL,
    GreaterOrEqual = GL_GEQUAL,
    Always = GL_ALWAYS
};

class OpenGLState
{
public:
    void EnableFaceCulling();
    void DisableFaceCulling();
    void SetFrontFaceRotation(FrontFaceRotation frontFace);
    void SetFaceCullSide(FaceCullSide faceSide);

    void EnableDepthTest();
    void DisableDepthTest();
    void EnableDepthWrite();
    void DisableDepthWrite();
    void SetDepthTestFunction(DepthTestFunction depthTestFunction);

    void SetShaderProgram(const Shader &shader);

    void Initialize();

private:
    int m_CurrentShaderProgram;

    bool m_EnableFaceCulling;
    FaceCullSide m_FaceCullSide;
    FrontFaceRotation m_FrontFace;

    bool m_DepthTestEnabled;
    bool m_DepthWriteEnabled;
    DepthTestFunction m_DepthTestFunction;
};


