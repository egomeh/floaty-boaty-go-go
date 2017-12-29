#pragma once

#include <memory>
#include <unordered_map>

#include "assetdatabase.hpp"
#include "graphics/texture.hpp"
#include "serialization/serialization.hpp"

const std::map<std::string, TextureFilterMode> textureFiltermodeWordMap =
{
    {"point", TextureFilterMode::Point},
    {"bilinear", TextureFilterMode::Bilinear},
    {"trilinear", TextureFilterMode::Trilinear},
};

const std::map<std::string, TextureWrapMode> textureWrapmodeWordMap =
{
    {"clamp", TextureWrapMode::Clamp},
    {"repeat", TextureWrapMode::Repeat},
};

class TextureAssetMetaData
{
public:
    TextureAssetMetaData();

    template<typename SerializationType>
    void Deserialize(SerializationType serializer)
    {
        std::string wrapmode;
        std::string filtermode;
        std::string format;

        DESERIALIZE(wrapmode, serializer);
        DESERIALIZE(filtermode, serializer);
        DESERIALIZE(format, serializer);
        DESERIALIZE(mipmaps, serializer);
        DESERIALIZE(flipy, serializer);
        DESERIALIZE(anisotropic, serializer);

        m_WrapMode = StringToEnum(wrapmode, textureWrapmodeWordMap, TextureWrapMode::Clamp);
        m_FilterMode = StringToEnum(filtermode, textureFiltermodeWordMap, TextureFilterMode::Bilinear);
    }

    TextureFilterMode m_FilterMode;
    TextureWrapMode m_WrapMode;
    TextureFormat m_TextureFormat;

    float anisotropic;

    bool mipmaps;
    bool flipy;
};

class TextureAssetFactory : public AssetFactory<Texture>
{
public:
    Texture *GetAsset(const std::string &name) override;
    void RefreshAsset(const std::string &name) override;

    std::vector<glm::vec4> GenerateErrorTextureData() const;
    std::vector<glm::vec4> GenerateMissingTextureData() const;
    void SetTextureAsError(Texture2D *texture) const;

private:
    std::shared_ptr<Texture2D> m_MissingTexture;
    std::unordered_map<std::string, std::shared_ptr<Texture2D>> m_LoadedAssets;
};
