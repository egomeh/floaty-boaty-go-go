#include "textureasset.hpp"

#include "serialization/jsonserializer.hpp"

#include "graphics/texture.hpp"
#include "graphics/graphicsutils.hpp"

#include <algorithm>
#include <vector>

#include "util.hpp"

#include <filesystem>

Texture *TextureAssetFactory::GetAsset(const std::string &name)
{
    if (m_LoadedAssets.count(name) != 0)
    {
        return m_LoadedAssets[name].get();
    }

    if (m_AssetSubMap->count(name) == 0)
    {
        m_MissingTexture = std::make_shared<Texture2D>(2, 2);

        auto errorTextureData = GenerateMissingTextureData();

        m_MissingTexture->SetFilterMode(TextureFilterMode::Point);
        m_MissingTexture->SetWrapMode(TextureWrapMode::Repeat);
        m_MissingTexture->SetAnisootropicFiltering(1.f);
        m_MissingTexture->SetPixels(errorTextureData);
        m_MissingTexture->GenerateMipmaps(true);

        m_MissingTexture->Create();

        m_MissingTexture->DiscardHostMemory();

        return m_MissingTexture.get();
    }

    m_LoadedAssets.insert(std::make_pair(name, std::make_shared<Texture2D>()));

    RefreshAsset(name);

    return m_LoadedAssets[name].get();
}

void TextureAssetFactory::RefreshAsset(const std::string &name)
{
    if (m_LoadedAssets.count(name) == 0)
    {
        return;
    }

    std::shared_ptr<Texture2D> texture = m_LoadedAssets[name];

    texture->Delete();

    nlohmann::json &textureAsset = (*m_AssetSubMap)[name];

    bool hasMeta = textureAsset["hasMeta"];

    std::string path = textureAsset["path"];

    if (m_AssetDependencyTracker)
    {
        m_AssetDependencyTracker->InsertDependency<Texture>(name, path);
    }

    TextureAssetMetaData meta;

    if (hasMeta)
    {
        std::string metaDataPath = path + ".meta";
        std::string metaData = m_ResourceLoader->LoadFileAsText(metaDataPath);

        if (m_AssetDependencyTracker)
        {
            m_AssetDependencyTracker->InsertDependency<Texture>(name, metaDataPath);
        }

        try
        {
            JsonSerializer serializer(metaData);
            meta.Deserialize(serializer);
        }
        catch (std::exception &ex)
        {
            SetTextureAsError(texture.get());
            DebugLog(ex.what());
            return;
        }
    }

    std::vector<uint8_t> textureData;

    m_ResourceLoader->LoadFileContent(path, textureData);

    int width, height, channels;

    std::vector<glm::vec4> pixelData;

    GetPixelsFromBinaryImage(textureData, pixelData, width, height, channels);

    texture->SetDimensions(width, height);

    if (meta.flipy)
    {
        for (int x = 0; x < width; ++x)
        {
            for (int y = 0; y < height / 2; ++y)
            {
                std::size_t origin = x + y * height;
                std::size_t target = x + ((height - 1) - y) * height;

                std::swap(pixelData[origin], pixelData[target]);
            }
        }
    }

    texture->SetFilterMode(meta.m_FilterMode);
    texture->SetWrapMode(meta.m_WrapMode);
    texture->SetAnisootropicFiltering(meta.anisotropic);
    texture->SetPixels(pixelData);

    texture->GenerateMipmaps(meta.mipmaps);

    texture->Create();

    texture->DiscardHostMemory();
}

std::vector<glm::vec4> TextureAssetFactory::GenerateErrorTextureData() const
{
    std::vector<glm::vec4> errorTextureData;

    errorTextureData.resize(4);

    errorTextureData[0] = glm::vec4(1.f, 0.f, 1.f, 1.f);
    errorTextureData[1] = glm::vec4(0.f, 0.f, 0.f, 1.f);
    errorTextureData[2] = glm::vec4(0.f, 0.f, 0.f, 1.f);
    errorTextureData[3] = glm::vec4(1.f, 0.f, 1.f, 1.f);

    return errorTextureData;
}

std::vector<glm::vec4> TextureAssetFactory::GenerateMissingTextureData() const
{
    std::vector<glm::vec4> errorTextureData;

    errorTextureData.resize(4);

    errorTextureData[0] = glm::vec4(1.f, 1.f, 0.f, 1.f);
    errorTextureData[1] = glm::vec4(0.f, 0.f, 0.f, 1.f);
    errorTextureData[2] = glm::vec4(0.f, 0.f, 0.f, 1.f);
    errorTextureData[3] = glm::vec4(1.f, 1.f, 0.f, 1.f);

    return errorTextureData;
}

void TextureAssetFactory::SetTextureAsError(Texture2D *texture) const
{
    auto errorTextureData = GenerateErrorTextureData();

    texture->Delete();
    texture->SetDimensions(2, 2);

    texture->SetFilterMode(TextureFilterMode::Point);
    texture->SetWrapMode(TextureWrapMode::Repeat);
    texture->SetAnisootropicFiltering(1.f);
    texture->SetPixels(errorTextureData);
    texture->GenerateMipmaps(true);

    texture->Create();

    texture->DiscardHostMemory();
}

TextureAssetMetaData::TextureAssetMetaData()
{
    m_FilterMode = TextureFilterMode::Point;
    m_WrapMode = TextureWrapMode::Clamp;
    m_TextureFormat = TextureFormat::RGBA32;
}
