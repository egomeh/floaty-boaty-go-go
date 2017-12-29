#pragma once

#include <algorithm>
#include <string>
#include <unordered_map>

struct AssetIdentifier
{
public:
    AssetIdentifier(std::size_t typeId, std::string assetName) : assetType(typeId), name(assetName)
    {
    }

    bool AssetIdentifier::operator==(const AssetIdentifier &other) const
    {
        bool namesAreEqual = name.compare(other.name) == 0;
        bool typesAreEqual = assetType == other.assetType;

        return namesAreEqual && typesAreEqual;
    }

    std::size_t assetType;
    std::string name;
};

namespace std
{
    template<>
    struct hash<AssetIdentifier>
    {
        size_t operator()(const AssetIdentifier &identifier) const
        {
            return std::hash<std::string>()(identifier.name) ^ std::hash<std::size_t>()(identifier.assetType);
        }
    };
}

class AssetDependencyTracker
{
public:
    AssetDependencyTracker();

    template<typename T>
    void InsertDependency(const std::string &assetName, const std::string &dependencyFilePath)
    {
        std::size_t typeId = GetTypeID<T>();
        InsertDependency(typeId, assetName, dependencyFilePath);
    }

    template<typename T>
    void InsertDependency(const std::string &assetName, const AssetIdentifier &asset)
    {
        const std::size_t typeId = GetTypeID<T>();
        InsertDependency(typeId, assetName, asset);
    }

    void InsertDependency(const std::size_t assetType, const std::string &assetName, const std::string &dependencyFilePath);
    void InsertDependency(const std::size_t assetType, const std::string &assetName, const AssetIdentifier &dependencyAsset);
    void UntrackAsset(const AssetIdentifier &asset);

    const auto &GetFileDependencies() const
    {
        return m_FileDependecyTriggers;
    }

    const auto &GetAssetDependencies() const
    {
        return m_AssetDependecyTriggers;
    }

private:
    std::unordered_map<std::string, std::vector<AssetIdentifier>> m_FileDependecyTriggers;
    std::unordered_map<AssetIdentifier, std::vector<AssetIdentifier>> m_AssetDependecyTriggers;
};
