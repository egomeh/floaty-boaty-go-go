#include "assetdependencytracker.hpp"

#include <sstream>

#include "util.hpp"

AssetDependencyTracker::AssetDependencyTracker()
{
}

void AssetDependencyTracker::InsertDependency(const std::size_t assetType, const std::string &assetName, const std::string &dependencyFilePath)
{
    AssetIdentifier identifier(assetType, assetName);

    auto currentTriggers = m_FileDependecyTriggers.find(dependencyFilePath);

    if (currentTriggers == m_FileDependecyTriggers.end())
    {
        std::vector<AssetIdentifier> newDependencyList;
        newDependencyList.push_back(identifier);

        auto newMapping = std::make_pair(dependencyFilePath, newDependencyList);

        m_FileDependecyTriggers.insert(newMapping);
    }
    else
    {
        auto &triggerList = currentTriggers->second;

        if (!Contains(triggerList, identifier))
        {
            triggerList.push_back(identifier);
        }
    }
}

void AssetDependencyTracker::InsertDependency(const std::size_t assetType, const std::string & assetName, const AssetIdentifier & dependencyAsset)
{
    AssetIdentifier identifier(assetType, assetName);

    auto currentTriggers = m_AssetDependecyTriggers.find(dependencyAsset);

    if (currentTriggers == m_AssetDependecyTriggers.end())
    {
        std::vector<AssetIdentifier> newDependencyList;
        newDependencyList.push_back(identifier);

        auto newMapping = std::make_pair(dependencyAsset, newDependencyList);

        m_AssetDependecyTriggers.insert(newMapping);
    }
    else
    {
        auto &triggerList = currentTriggers->second;

        if (!Contains(triggerList, identifier))
        {
            triggerList.push_back(identifier);
        }
    }
}

void AssetDependencyTracker::UntrackAsset(const AssetIdentifier &asset)
{
    for (auto &fileDepedency : m_FileDependecyTriggers)
    {
        std::remove(fileDepedency.second.begin(), fileDepedency.second.end(), asset);
    }

    for (auto &assetDepedency : m_AssetDependecyTriggers)
    {
        std::remove(assetDepedency.second.begin(), assetDepedency.second.end(), asset);
    }
}
