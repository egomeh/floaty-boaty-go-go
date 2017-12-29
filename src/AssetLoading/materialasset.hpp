#pragma once

#include "graphics/material.hpp"

#include <string>

#include "assetdatabase.hpp"

class MaterialAssetFactory : public AssetFactory<Material>
{
public:
    MaterialAssetFactory();

    Material *GetAsset(const std::string &name) override;
    void RefreshAsset(const std::string &name) override;

private:
    std::unordered_map<std::string, std::shared_ptr<Material>> m_LoadedAssets;
};
