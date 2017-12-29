#pragma once

#include <memory>
#include <unordered_map>

#include "assetdatabase.hpp"
#include "serialization/serialization.hpp"
#include "scene.hpp"

class SceneAssetFactory : public AssetFactory<SceneData>
{
public:
    SceneData *GetAsset(const std::string &name) override;

private:
    std::unordered_map<std::string, std::shared_ptr<SceneData>> m_LoadedAssets;
};

class PrefabAssetFactory : public AssetFactory<Prefab>
{
public:
    Prefab *GetAsset(const std::string &name) override;

private:
    std::unordered_map<std::string, std::shared_ptr<Prefab>> m_LoadedAssets;
};
