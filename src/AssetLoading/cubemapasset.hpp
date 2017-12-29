#pragma once

#include <memory>
#include <unordered_map>

#include "assetdatabase.hpp"
#include "graphics/texture.hpp"
#include "serialization/serialization.hpp"

namespace CubemapFaceKeywords
{
    static const std::string Right("right");
    static const std::string Left("left");
    static const std::string Top("top");
    static const std::string Bottom("bottom");
    static const std::string Back("back");
    static const std::string Front("front");
}

const std::map<std::string, CubemapFace> cubemapFaceMap =
{
    {CubemapFaceKeywords::Right, Right},
    {CubemapFaceKeywords::Left, Left},
    {CubemapFaceKeywords::Top, Top},
    {CubemapFaceKeywords::Bottom, Bottom},
    {CubemapFaceKeywords::Back, Back},
    {CubemapFaceKeywords::Front, Front},
};

class CubemapAssetFactory : public AssetFactory<Cubemap>
{
public:
    Cubemap *GetAsset(const std::string &name) override;

    void SetTextureAssets(nlohmann::json &textureAssets);

private:
    std::unordered_map<std::string, std::shared_ptr<Cubemap>> m_LoadedAssets;
    nlohmann::json m_TextureAssets;
};
