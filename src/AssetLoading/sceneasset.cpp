#include "sceneasset.hpp"

#include "serialization/jsonserializer.hpp"

#include "graphics/mesh.hpp"
#include "graphics/graphicsutils.hpp"

#include "util.hpp"

#include <algorithm>
#include <vector>

SceneData *SceneAssetFactory::GetAsset(const std::string &name)
{
    if (m_LoadedAssets.count(name) != 0)
    {
        return m_LoadedAssets[name].get();
    }

    nlohmann::json &sceneasset = (*m_AssetSubMap)[name];

    std::string path = sceneasset["path"];

    std::string serializedScene = m_ResourceLoader->LoadFileAsText(path);

    nlohmann::json sceneData = nlohmann::json::parse(serializedScene);

    std::shared_ptr<SceneData> scene = std::make_shared<SceneData>();

    scene->entities = sceneData["entities"];

    m_LoadedAssets.insert(std::make_pair(name, scene));

    return scene.get();
}

Prefab * PrefabAssetFactory::GetAsset(const std::string &name)
{
    if (m_LoadedAssets.count(name) != 0)
    {
        return m_LoadedAssets[name].get();
    }

    nlohmann::json &sceneasset = (*m_AssetSubMap)[name];

    std::string path = sceneasset["path"];

    std::string serializedScene = m_ResourceLoader->LoadFileAsText(path);

    nlohmann::json sceneData = nlohmann::json::parse(serializedScene);

    std::shared_ptr<Prefab> prefab = std::make_shared<Prefab>();

    prefab->content = sceneData;

    m_LoadedAssets.insert(std::make_pair(name, prefab));

    return prefab.get();
}
