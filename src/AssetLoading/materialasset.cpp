#include "materialasset.hpp"

#include <algorithm>

#include "graphics/shader.hpp"

MaterialAssetFactory::MaterialAssetFactory()
{
}

Material *MaterialAssetFactory::GetAsset(const std::string &name)
{
    if (m_LoadedAssets.count(name) != 0)
    {
        return m_LoadedAssets[name].get();
    }

    std::shared_ptr<Material> material = std::make_shared<Material>();

    m_LoadedAssets.insert(std::make_pair(name, material));

    RefreshAsset(name);

    return material.get();
}

void MaterialAssetFactory::RefreshAsset(const std::string & name)
{
    auto &loadedMaterial = m_LoadedAssets.find(name);

    if (loadedMaterial == m_LoadedAssets.end())
    {
        return;
    }

    std::shared_ptr<Material> material = loadedMaterial->second;

    nlohmann::json &materialAsset = (*m_AssetSubMap)[name];

    std::string path = materialAsset["path"];

    if (m_AssetDependencyTracker)
    {
        m_AssetDependencyTracker->InsertDependency<Material>(name, path);
    }

    std::string serializedMaterial = m_ResourceLoader->LoadFileAsText(path);

    nlohmann::json materialJson;

    try
    {
        materialJson = nlohmann::json::parse(serializedMaterial);
    }
    catch (std::exception &ex)
    {
        DebugLog(ex.what());
        return;
    }

    std::string shaderName = materialJson["shader"];

    Shader *shader = m_AssetDatabase->RequestAsset<Shader>(shaderName);

    material->SetShader(shader);

    if (m_AssetDependencyTracker)
    {
        AssetIdentifier shaderAssetIdentifier(GetTypeID<Shader>(), shaderName);
        m_AssetDependencyTracker->InsertDependency<Material>(name, shaderAssetIdentifier);
    }

    if (materialJson.count("uniforms") != 0)
    {
        const UniformPropertySet &uniformData = material->GetUniformContext();

        const nlohmann::json &uniforms = materialJson["uniforms"];

        auto uniformIterator = uniforms.begin();

        for (; uniformIterator != uniforms.end(); ++uniformIterator)
        {
            const std::string &uniformName = uniformIterator.key();

            std::size_t uniformHash = std::hash<std::string>()(uniformName);

            auto &textureUniforms = uniformData.GetTextures();

            auto textureUniform = textureUniforms.find(uniformHash);

            if (textureUniform != textureUniforms.end())
            {
                const std::string &textureName = uniforms[uniformName];

                Texture *texture;

                if (textureUniform->second.GetUniformType() == UniformType::UniformSampler2D)
                {
                    texture = m_AssetDatabase->RequestAsset<Texture>(textureName);
                }
                else
                {
                    texture = m_AssetDatabase->RequestAsset<Cubemap>(textureName);
                }

                material->GetUniformContext().SetTexture(uniformHash, texture);
            }
        }
    }

    m_LoadedAssets.insert(std::make_pair(name, material));
}
