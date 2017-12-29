#include "shader.hpp"
#include "shaderpreprocessor.hpp"
#include "graphicsutils.hpp"

Shader::Shader(std::string name) : m_Compiled(false), m_ShaderId(0), m_RenderQueue(RenderQueue::Default)
{
    m_Name = name;
}

Shader::~Shader()
{
    DeleteShader();
}

void Shader::SetProperties(ShaderProperties properties)
{
    m_SaderProperties = properties;
}

ShaderBlendMode Shader::GetBlendMode() const
{
    return m_SaderProperties.GetBlendMode();
}

RenderQueue Shader::GetRenderQueue() const
{
    return m_RenderQueue;
}


bool Shader::Compile(std::string vertexSource, std::string fragmentSource)
{
    GLuint vertexShaderObject;
    GLuint fragmentShaderObject;

    const char *vertexSourceCstr = vertexSource.c_str();
    vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);
    GL_ERROR_CHECK();

    glShaderSource(vertexShaderObject, 1, &vertexSourceCstr, NULL);
    GL_ERROR_CHECK();

    glCompileShader(vertexShaderObject);
    GL_ERROR_CHECK();

    GLint success;
    GLint logLength;
    std::vector<char> log;

    glGetShaderiv(vertexShaderObject, GL_COMPILE_STATUS, &success);
    GL_ERROR_CHECK();

    if (success == GL_FALSE)
    {
        glGetShaderiv(vertexShaderObject, GL_INFO_LOG_LENGTH, &logLength);
        GL_ERROR_CHECK();

        log.resize(logLength);

        glGetShaderInfoLog(vertexShaderObject, logLength, &logLength, log.data());
        GL_ERROR_CHECK();

        m_ErrorLog = std::string(log.begin(), log.end());

        glDeleteShader(vertexShaderObject);
        GL_ERROR_CHECK();

        return false;
    }

    const char *fragmentSourceCstr = fragmentSource.c_str();

    fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);
    GL_ERROR_CHECK();

    glShaderSource(fragmentShaderObject, 1, &fragmentSourceCstr, NULL);
    GL_ERROR_CHECK();

    glCompileShader(fragmentShaderObject);
    GL_ERROR_CHECK();

    glGetShaderiv(fragmentShaderObject, GL_COMPILE_STATUS, &success);
    GL_ERROR_CHECK();

    if (success == GL_FALSE)
    {
        glGetShaderiv(fragmentShaderObject, GL_INFO_LOG_LENGTH, &logLength);
        GL_ERROR_CHECK();

        log.resize(logLength);

        glGetShaderInfoLog(fragmentShaderObject, logLength, &logLength, log.data());
        GL_ERROR_CHECK();

        m_ErrorLog = std::string(log.begin(), log.end());

        glDeleteShader(vertexShaderObject);
        GL_ERROR_CHECK();

        glDeleteShader(fragmentShaderObject);
        GL_ERROR_CHECK();

        return false;
    }

    GLuint shaderProgram = glCreateProgram();
    shaderProgram = glCreateProgram();
    GL_ERROR_CHECK();

    glAttachShader(shaderProgram, vertexShaderObject);
    GL_ERROR_CHECK();

    glAttachShader(shaderProgram, fragmentShaderObject);
    GL_ERROR_CHECK();

    glLinkProgram(shaderProgram);
    GL_ERROR_CHECK();

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    GL_ERROR_CHECK();

    if (success == GL_FALSE)
    {
        glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &logLength);
        GL_ERROR_CHECK();

        log.resize(logLength);

        glGetProgramInfoLog(shaderProgram, logLength, &logLength, log.data());
        GL_ERROR_CHECK();

        m_ErrorLog = std::string(log.begin(), log.end());
        m_ErrorOrigin = m_Name;

        glDeleteShader(vertexShaderObject);
        GL_ERROR_CHECK();

        glDeleteShader(fragmentShaderObject);
        GL_ERROR_CHECK();

        glDeleteProgram(shaderProgram);
        GL_ERROR_CHECK();

        return false;
    }

    glDetachShader(shaderProgram, vertexShaderObject);
    GL_ERROR_CHECK();

    glDetachShader(shaderProgram, fragmentShaderObject);
    GL_ERROR_CHECK();

    glDeleteShader(vertexShaderObject);
    GL_ERROR_CHECK();

    glDeleteShader(fragmentShaderObject);
    GL_ERROR_CHECK();

    GLuint oldShader = m_ShaderId;
    m_ShaderId = shaderProgram;

    if (oldShader != 0)
    {
        glDeleteProgram(oldShader);
        GL_ERROR_CHECK();
    }

    m_Compiled = true;

    GetUniforms();

    return true;
}

GLuint Shader::GetShaderId() const
{
    return m_ShaderId;
}

std::string Shader::GetErrorLog() const
{
    return m_ErrorLog;
}

std::string Shader::GetErrorOrigin() const
{
    return m_ErrorOrigin;
}

void Shader::DisownProgram()
{
    m_ShaderId = 0;
}

int Shader::GetUniformLocation(std::string uniform) const
{
    if (m_Compiled)
    {
        GLint location = glGetUniformLocation(m_ShaderId, uniform.c_str());
        GL_ERROR_CHECK();

        return static_cast<int>(location);
    }
    return -1;
}

void Shader::TakeProgramOwnership(std::shared_ptr<Shader> other)
{
    if (other->GetShaderId() != 0)
    {
        m_Compiled = true;
    }

    m_ShaderId = other->GetShaderId();
    other->DisownProgram();
}

std::vector<ShaderVariableInfo> Shader::GetUniforms() const
{
    std::vector<ShaderVariableInfo> uniforms;

    if (!m_Compiled)
    {
        return uniforms;
    }

    GLint nUniforms;

    GLint uniformSize;
    GLenum uniformType;

    const GLsizei bufferSize = 255; // Maybe make this dynamic?
    GLchar uniformName[bufferSize]; // variable name in GLSL
    GLsizei nameLength; // name length

    m_ShaderId;

    glGetProgramiv(m_ShaderId, GL_ACTIVE_UNIFORMS, &nUniforms);
    GL_ERROR_CHECK();

    glUseProgram(m_ShaderId);
    GL_ERROR_CHECK();

    for (GLint i = 0; i < nUniforms; ++i)
    {
        glGetActiveUniform(m_ShaderId, static_cast<GLuint>(i), bufferSize, &nameLength, &uniformSize, &uniformType, uniformName);
        GL_ERROR_CHECK();

        GLint location = GetUniformLocation(uniformName);

        ShaderVariableInfo uniformInfo;

        uniformInfo.location = location;
        uniformInfo.name = uniformName;
        uniformInfo.variableType = OpenGLToHostType(uniformType);

        uniforms.push_back(uniformInfo);
    }

    return uniforms;
}

void Shader::DeleteShader()
{
    if (m_ShaderId != 0)
    {
        glDeleteProgram(m_ShaderId);
        GL_ERROR_CHECK();
    }
}

ShaderProperties::ShaderProperties() : m_BlendMode(Off)
{
}

ShaderBlendMode ShaderProperties::GetBlendMode() const
{
    return m_BlendMode;
}

void ShaderProperties::SetBlendMode(ShaderBlendMode blendMode)
{
    m_BlendMode = blendMode;
}

RenderQueue ShaderProperties::GetRenderQueue() const
{
    return m_RenderQueue;
}

void ShaderProperties::SetRenderQueue(RenderQueue queue)
{
    m_RenderQueue = queue;
}
