#pragma once

#include <memory>
#include <unordered_map>

#include "assetdatabase.hpp"
#include "graphics/font.hpp"
#include "serialization/serialization.hpp"

const std::map<std::string, FontTextureType> fontRenderingType =
{
    {"raster", FontTextureType::Raster},
    {"sdf", FontTextureType::SignedDistanceField},
    {"mcsdf", FontTextureType::MultiChannelSignedDistanceField},
};

class FontAssetFactory : public AssetFactory<Font>
{
public:
    Font *GetAsset(const std::string &name) override;
    void RefreshAsset(const std::string &name) override;

private:
    std::unordered_map<std::string, std::shared_ptr<Font>> m_LoadedAssets;
};
