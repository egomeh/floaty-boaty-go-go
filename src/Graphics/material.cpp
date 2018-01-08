#include "material.hpp"

Material::Material(Shader *p_Shader) : m_UniformProperties(p_Shader->GetUniforms()), m_Shader(p_Shader), m_RenderQueue(p_Shader->GetRenderQueue())
{
}

Material::Material(): m_Shader(nullptr)
{
}

Material::~Material()
{
}

UniformPropertySet &Material::GetUniformContext()
{
    return m_UniformProperties;
}

const UniformPropertySet & Material::GetUniformContext() const
{
    return m_UniformProperties;
}

void Material::SetShader(const Shader *p_Shader)
{
    m_Shader = p_Shader;
    m_UniformProperties = UniformPropertySet(m_Shader->GetUniforms());
}

void Material::SetBlendMode(ShaderBlendMode blendMode)
{
    m_ShaderBlendMode = blendMode;
}

void Material::UpdateShaderVariableMaps()
{
}

const Shader *Material::GetShader() const
{
    return m_Shader;
}

UniformPropertySet::UniformPropertySet(std::vector<ShaderVariableInfo> shaderVariables)
{
    while (shaderVariables.size() > 0)
    {
        ShaderVariableInfo variableInfo = shaderVariables.back();
        shaderVariables.pop_back();

        std::size_t propertyHash = std::hash<std::string>()(variableInfo.name);

        switch (variableInfo.variableType)
        {
            case (UniformInt):
            {
                m_Ints[propertyHash] = UniformProperty<int>(variableInfo.location, 0);
                break;
            }
            case (UniformFloat):
            {
                m_Floats[propertyHash] = UniformProperty<float>(variableInfo.location, 0.f);
                break;
            }
            case (UniformVector2):
            {
                m_Vec2s[propertyHash] = UniformProperty<glm::vec2>(variableInfo.location, glm::vec2(0));
                break;
            }
            case (UniformVector3):
            {
                m_Vec3s[propertyHash] = UniformProperty<glm::vec3>(variableInfo.location, glm::vec3(0));
                break;
            }
            case (UniformVector4):
            {
                m_Vec4s[propertyHash] = UniformProperty<glm::vec4>(variableInfo.location, glm::vec4(0));
                break;
            }
            case (UniformMatrix3x3):
            {
                m_Mat3s[propertyHash] = UniformProperty<glm::mat3>(variableInfo.location, glm::mat3(1));
                break;
            }
            case (UniformMatrix4x4):
            {
                m_Mat4s[propertyHash] = UniformProperty<glm::mat4>(variableInfo.location, glm::mat4(1));
                break;
            }
            case (UniformSampler2D):
            {
                m_Textures[propertyHash] = TextureUniformProperty(variableInfo.location, UniformSampler2D);
                break;
            }
            case (UniformSamplerCube):
            {
                m_Textures[propertyHash] = TextureUniformProperty(variableInfo.location, UniformSamplerCube);
                break;
            }
        }
    }
}

UniformPropertySet::UniformPropertySet()
{
}

void UniformPropertySet::SetInt(std::size_t nameHash, int value)
{
    auto iterator = m_Ints.find(nameHash);
    if (iterator != m_Ints.end())
    {
        iterator->second.SetValue(value);
    }
}

void UniformPropertySet::SetFloat(std::size_t nameHash, float value)
{
    auto iterator = m_Floats.find(nameHash);
    if (iterator != m_Floats.end())
    {
        iterator->second.SetValue(value);
    }
}

void UniformPropertySet::SetVector2(std::size_t nameHash, glm::vec2 value)
{
    auto iterator = m_Vec2s.find(nameHash);
    if (iterator != m_Vec2s.end())
    {
        iterator->second.SetValue(value);
    }
}

void UniformPropertySet::SetVector3(std::size_t nameHash, glm::vec3 value)
{
    auto iterator = m_Vec3s.find(nameHash);
    if (iterator != m_Vec3s.end())
    {
        iterator->second.SetValue(value);
    }
}

void UniformPropertySet::SetVector4(std::size_t nameHash, glm::vec4 value)
{
    auto iterator = m_Vec4s.find(nameHash);
    if (iterator != m_Vec4s.end())
    {
        iterator->second.SetValue(value);
    }
}

void UniformPropertySet::SetMat3(std::size_t nameHash, glm::mat3 value)
{
    auto iterator = m_Mat3s.find(nameHash);
    if (iterator != m_Mat3s.end())
    {
        iterator->second.SetValue(value);
    }
}

void UniformPropertySet::SetMat4(std::size_t nameHash, glm::mat4 value)
{
    auto iterator = m_Mat4s.find(nameHash);
    if (iterator != m_Mat4s.end())
    {
        iterator->second.SetValue(value);
    }
}

void UniformPropertySet::SetTexture(std::size_t nameHash, Texture *value)
{
    auto iterator = m_Textures.find(nameHash);
    if (iterator != m_Textures.end())
    {
        iterator->second.SetValue(value);
    }
}

void TextureUniformProperty::SetValue(Texture * value)
{
    m_Value = value;
}
