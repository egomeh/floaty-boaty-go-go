#pragma once

#include "includeglew.hpp"

#include <typeinfo>
#include "glm/glm.hpp"

#include <string>

#ifdef CHECK_OPENGL_ERRORS
#include "../util.hpp"
#include <sstream>

inline void PrintOpenGLErrors(const std::string &file, int line)
{
    GLenum errorCode;
    const GLubyte *errorString;
    while ((errorCode = glGetError()) != GL_NO_ERROR)
    {
        errorString = gluErrorString(errorCode);
        std::stringstream errorStream;
        errorStream << "OpenGL: " << file << " line " << line << " ";
        errorStream << errorString;
        DebugLog(errorStream.str());
    }
}

#define GL_ERROR_CHECK() PrintOpenGLErrors(__FILE__, __LINE__)

#else
#define GL_ERROR_CHECK()
#endif

enum UniformType
{
    UniformUnkownType,
    UniformInt,
    UniformFloat,
    UniformVector2,
    UniformVector3,
    UniformVector4,
    UniformMatrix3x3,
    UniformMatrix4x4,
    UniformSampler2D,
    UniformSamplerCube
};

static UniformType OpenGLToHostType(GLenum type)
{
    switch (type)
    {
        case (GL_INT):
            return UniformInt;
        case (GL_FLOAT):
            return UniformFloat;
        case (GL_FLOAT_VEC2):
            return UniformVector2;
        case (GL_FLOAT_VEC3):
            return UniformVector3;
        case (GL_FLOAT_VEC4):
            return UniformVector4;
        case (GL_FLOAT_MAT3):
            return UniformMatrix3x3;
        case (GL_FLOAT_MAT4):
            return UniformMatrix4x4;
        case (GL_SAMPLER_2D):
            return UniformSampler2D;
        case (GL_SAMPLER_CUBE):
            return UniformSamplerCube;
        default:
            return UniformUnkownType;
    }
}

static std::string UniformTypeName(UniformType uniformType)
{
    switch (uniformType)
    {
        case (UniformInt):
            return "Integer";
        case (UniformFloat):
            return "Float";
        case (UniformVector2):
            return "vec2";
        case (UniformVector3):
            return "vec3";
        case (UniformVector4):
            return "vec4";
        case (UniformMatrix3x3):
            return "mat3x3";
        case (UniformMatrix4x4):
            return "mat4x4";
        case (UniformSampler2D):
            return "Sampler2D";
        case (UniformSamplerCube):
            return "SamplerCube";
        default:
            return "Unkown";
    }
}

