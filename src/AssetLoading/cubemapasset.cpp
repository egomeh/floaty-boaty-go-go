#include "cubemapasset.hpp"

#include "serialization/jsonserializer.hpp"

#include "graphics/texture.hpp"
#include "graphics/graphicsutils.hpp"

#include "util.hpp"

#include "textureasset.hpp"

#include <algorithm>
#include <vector>

Cubemap *CubemapAssetFactory::GetAsset(const std::string &name)
{
    if (m_LoadedAssets.count(name) != 0)
    {
        return m_LoadedAssets[name].get();
    }

    std::shared_ptr<Cubemap> p_Cubemap = std::make_shared<Cubemap>();

    nlohmann::json &cubemapAsset = (*m_AssetSubMap)[name];

    std::string path = cubemapAsset["path"];

    std::string serializedCubemapData = m_ResourceLoader->LoadFileAsText(path);

    nlohmann::json cubemapData = nlohmann::json::parse(serializedCubemapData);

    nlohmann::json &faces = cubemapData["faces"];

    TextureAssetMetaData meta;

    JsonSerializer serializer(cubemapData);

    meta.Deserialize(serializer);

    for (auto it = faces.begin(); it != faces.end(); ++it)
    {
        const std::string faceName = it.key();
        const std::string textureAssetName = it.value();

        CubemapFace cubemapFace = StringToEnum(faceName, cubemapFaceMap, Right);

        std::vector<uint8_t> pixelData;

        nlohmann::json textureAssetInfo = (m_TextureAssets)[textureAssetName];

        std::string texturePath = textureAssetInfo["path"];

        std::vector<uint8_t> binaryImageData;

        m_ResourceLoader->LoadFileContent(texturePath, binaryImageData);

        std::vector<glm::vec4> texturePixelData;

        int width, height, channels;
        
        GetPixelsFromBinaryImage(binaryImageData, texturePixelData, width, height, channels);

        p_Cubemap->SetPixelDataForFace(texturePixelData, width, height, cubemapFace);
    }

    p_Cubemap->SetFilterMode(meta.m_FilterMode);
    p_Cubemap->SetWrapMode(meta.m_WrapMode);
    p_Cubemap->SetAnisootropicFiltering(meta.anisotropic);

    p_Cubemap->Create();

    p_Cubemap->DiscardHostData();

    m_LoadedAssets.insert(std::make_pair(name, p_Cubemap));

    return p_Cubemap.get();
}

void CubemapAssetFactory::SetTextureAssets(nlohmann::json &textureAssets)
{
    m_TextureAssets = textureAssets;
}
