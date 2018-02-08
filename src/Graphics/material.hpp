#pragma once

#include <map>
#include <unordered_map>

#include <functional>

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "graphics.hpp"
#include "shader.hpp"
#include "texture.hpp"

template<typename T>
class UniformProperty
{
public:
    UniformProperty(int location, T initialValue)
    {
        m_Location = location;
        m_Value = initialValue;
    }
    UniformProperty() : m_Location(-1) {}

    void SetValue(T value)
    {
        m_Value = value;
    }

    const T &GetValue() const
    {
        return m_Value;
    }

    int GetLocation() const
    {
        return m_Location;
    }

private:
    int m_Location;
    T m_Value;
};

class TextureUniformProperty
{
public:
    TextureUniformProperty(int location, UniformType uniformType)
    {
        m_Location = location;
        m_HasExtraInfo = false;
        m_Value = nullptr;
        m_UniformType = uniformType;
    }
    TextureUniformProperty() : m_Location(-1) {}

    void SetValue(Texture *value);

    const Texture *GetValue() const
    {
        return m_Value;
    }

    int GetLocation() const
    {
        return m_Location;
    }

    UniformType GetUniformType() const
    {
        return m_UniformType;
    }

private:
    int m_Location;
    Texture *m_Value;
    bool m_HasExtraInfo;
    UniformType m_UniformType;
};

class UniformPropertySet
{
public:
    UniformPropertySet(std::vector<ShaderVariableInfo> shaderVariables);
    UniformPropertySet();

    void SetInt(std::size_t, int value);
    void SetFloat(std::size_t, float value);
    void SetVector2(std::size_t, glm::vec2 value);
    void SetVector3(std::size_t, glm::vec3 value);
    void SetVector4(std::size_t, glm::vec4 value);
    void SetMat3(std::size_t, glm::mat3);
    void SetMat4(std::size_t, glm::mat4);
    void SetTexture(std::size_t, Texture *value);

    const std::unordered_map<std::size_t, UniformProperty<int>> &GetInts() const
    {
        return m_Ints;
    }

    int GetInt(std::size_t hash) const
    {
        auto intProperty = m_Ints.find(hash);
        if (intProperty != m_Ints.end())
        {
            return intProperty->second.GetValue();
        }
    }

    const std::unordered_map<std::size_t, UniformProperty<float>> &GetFloats() const
    {
        return m_Floats;
    }

    const std::unordered_map<std::size_t, UniformProperty<glm::vec2>> &GetVec2s() const
    {
        return m_Vec2s;
    }

    const std::unordered_map<std::size_t, UniformProperty<glm::vec3>> &GetVec3s() const
    {
        return m_Vec3s;
    }

    const std::unordered_map<std::size_t, UniformProperty<glm::vec4>> &GetVec4s() const
    {
        return m_Vec4s;
    }

    const std::unordered_map<std::size_t, UniformProperty<glm::mat3>> &GetMat3s() const
    {
        return m_Mat3s;
    }

    const std::unordered_map<std::size_t, UniformProperty<glm::mat4>> &GetMat4s() const
    {
        return m_Mat4s;
    }

    const std::unordered_map<std::size_t, TextureUniformProperty> &GetTextures() const
    {
        return m_Textures;
    }

private:
    std::unordered_map<std::size_t, UniformProperty<int>> m_Ints;
    std::unordered_map<std::size_t, UniformProperty<float>> m_Floats;
    std::unordered_map<std::size_t, UniformProperty<glm::vec2>> m_Vec2s;
    std::unordered_map<std::size_t, UniformProperty<glm::vec3>> m_Vec3s;
    std::unordered_map<std::size_t, UniformProperty<glm::vec4>> m_Vec4s;
    std::unordered_map<std::size_t, UniformProperty<glm::mat3>> m_Mat3s;
    std::unordered_map<std::size_t, UniformProperty<glm::mat4>> m_Mat4s;
    std::unordered_map<std::size_t, TextureUniformProperty> m_Textures;
};

class Material
{
public:
    Material(Shader *p_Shader);
    Material();
    ~Material();

    UniformPropertySet &GetUniformContext();
    const UniformPropertySet &GetUniformContext() const;

    void SetShader(const Shader *p_Shader);

    void SetBlendMode(ShaderBlendMode blendMode);
    ShaderBlendMode GetBlendMode() const;

    void UpdateShaderVariableMaps();

    const Shader *GetShader() const;

private:
    ShaderBlendMode m_ShaderBlendMode;

    // Maps that stores the uniforms used by the shader in the material
    UniformPropertySet m_UniformProperties;

    RenderQueue m_RenderQueue;

    const Shader *m_Shader;
};
