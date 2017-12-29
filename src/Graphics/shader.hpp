#pragma once

#include <string>
#include <map>
#include <vector>
#include <typeinfo>

#include "graphics.hpp"
#include "graphicsutils.hpp"

struct ShaderVariableInfo
{
    int location;
    std::string name;
    UniformType variableType;
};

enum ShaderType
{
    Vertex,
    Fragment
};

enum ShaderBlendMode
{
    Off,
    Add,
    Alpha,
    Subtract,
    Multiply
};

typedef std::pair<std::string, std::string> ShaderSource;
typedef std::map<ShaderType, std::vector<ShaderSource>> ShaderSourceMap;

class ShaderProperties
{
public:
    ShaderProperties();

    ShaderBlendMode GetBlendMode() const;
    void SetBlendMode(ShaderBlendMode blendMode);

    RenderQueue GetRenderQueue() const;
    void SetRenderQueue(RenderQueue queue);
private:
    ShaderBlendMode m_BlendMode;
    RenderQueue m_RenderQueue;
};

class Shader
{
public:
    Shader(std::string name);
    ~Shader();

    void SetProperties(ShaderProperties properties);

    ShaderBlendMode GetBlendMode() const;
    RenderQueue GetRenderQueue() const;
    void DisownProgram();

    int GetUniformLocation(std::string uniform) const;
    
    void TakeProgramOwnership(std::shared_ptr<Shader> other);

    bool Compile(std::string vertexSource, std::string fragmentProgram);

    GLuint GetShaderId() const;

    std::string GetErrorLog() const;
    std::string GetErrorOrigin() const;

    // Shaders may not be copied
    Shader() = delete;
    Shader(const Shader &other) = delete;

    std::vector<ShaderVariableInfo> GetUniforms() const;

private:
    void DeleteShader();

private:
    bool m_Compiled;
    GLuint m_ShaderId;

    std::string m_ErrorLog;
    std::string m_ErrorOrigin;
    std::string m_Name;

    ShaderProperties m_SaderProperties;

    RenderQueue m_RenderQueue;
};

