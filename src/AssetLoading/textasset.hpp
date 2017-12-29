#pragma once

#include <memory>
#include <unordered_map>

#include "assetdatabase.hpp"
#include "graphics/shader.hpp"
#include "serialization/serialization.hpp"

class GLSL
{
public:
    std::string content;
};

class GlslAssetFactory : public AssetFactory<GLSL>
{
public:
    GLSL *GetAsset(const std::string &name) override;
    void RefreshAsset(const std::string &name) override;

private:
    std::unordered_map<std::string, std::shared_ptr<GLSL>> m_LoadedAssets;
};

