#pragma once

#include <unordered_map>
#include <string>
#include <memory>

#include "util.hpp"
#include "json.hpp"

#include "resources.hpp"

#include "ecs.hpp"

#include "AssetLoading/assetdependencytracker.hpp"

class AssetDatabase;

class AssetFactoryBase
{
public:
    virtual void RefreshAsset(const std::string &name)
    {
    }
};

class ComponentLoaderBase
{
public:
    virtual TypedComponentHandle LoadComponent(const nlohmann::json &serializedComponent) = 0;
    virtual TypedComponentHandle LoadComponent(ComponentTypeId typeId) = 0;
    virtual ComponentBase *GetAbstractComponent(ComponentHandle) = 0;

    void SetAsseDatabase(AssetDatabase *assetDatabase);
protected:
    AssetDatabase *m_AssetDatabase;
};

template<typename T>
class AssetFactory : public AssetFactoryBase
{
public:
    virtual T *GetAsset(const std::string &name) = 0;

    void SetAssetTypeName(std::string &assetTypeName)
    {
        m_AssetTypeName = assetTypeName;
    }

    void SetAssetDependencyTracker(AssetDependencyTracker *tracker)
    {
        m_AssetDependencyTracker = tracker;
    }

    std::string &GetAssetTypeName() const
    {
        return m_AssetTypeName;
    }

    void SetAssetSubMap(nlohmann::json *map)
    {
        m_AssetSubMap = map;
    }

    void SetResourceLoader(ResourceLoader *resourceLoader)
    {
        m_ResourceLoader = resourceLoader;
    }

    void SetAssetDatabase(AssetDatabase *assetDatabase)
    {
        m_AssetDatabase = assetDatabase;
    }

protected:
    std::string m_AssetTypeName;
    ResourceLoader *m_ResourceLoader;
    nlohmann::json *m_AssetSubMap;
    AssetDatabase *m_AssetDatabase;
    AssetDependencyTracker *m_AssetDependencyTracker = nullptr;
};

template<typename T>
class ComponentLoader : public ComponentLoaderBase
{
public:
    virtual TypedComponentHandle LoadComponent(const nlohmann::json &serializedComponent) override = 0;
    virtual TypedComponentHandle LoadComponent(ComponentTypeId typeId) override = 0;

    ComponentTypeId GetTargetComponentId()
    {
        return GetComponentTypeID<T>();
    }
};

class AssetDatabase
{
public:
    AssetDatabase();

    void SetAssetMapping(std::string mappingString);
    void SetReesourceLoader(ResourceLoader resourceLoader);

    template<typename T>
    void RegisterAssetType(std::string assetType, AssetFactoryBase *assetFactory)
    {
        std::size_t typeId = GetTypeID<T>();

        m_AssetTypeMap.insert(std::make_pair(typeId, assetFactory));

        AssetFactory<T> *typedAssetFactory = static_cast<AssetFactory<T>*>(assetFactory);
        
        typedAssetFactory->SetAssetTypeName(assetType);
        typedAssetFactory->SetAssetSubMap(&m_AssetMapping[assetType]);
        typedAssetFactory->SetResourceLoader(&m_ResourceLoader);
        typedAssetFactory->SetAssetDatabase(this);
    }

    template<typename T>
    T* RequestAsset(std::string assetName)
    {
        std::size_t typeId = GetTypeID<T>();

        if (m_AssetTypeMap.count(typeId) == 0)
        {
            return nullptr;
        }

        AssetFactoryBase *assetFactory = m_AssetTypeMap[typeId];

        AssetFactory<T> &typedAssetFactory = static_cast<AssetFactory<T>&>(*assetFactory);

        return typedAssetFactory.GetAsset(assetName);
    }

    AssetFactoryBase *GetAbstractAssetFactory(std::size_t typeId);

    nlohmann::json GetAssetSubMap(std::string subMap);

private:
    std::unordered_map<std::size_t, AssetFactoryBase*> m_AssetTypeMap;

    ResourceLoader m_ResourceLoader;

    nlohmann::json m_AssetMapping;
};

class ComponentLoaderSystem
{
public:
    void SetAssetDatabase(AssetDatabase *assetDatabase);

    template<typename T>
    void RegisterComponentLoader(std::string componentType, ComponentLoaderBase *componentLoader)
    {
        const ComponentTypeId typeId = GetComponentTypeID<T>();

        RegisterComponentLoader(componentType, typeId, componentLoader);
    }

    void RegisterComponentLoader(std::string componentType, ComponentTypeId typeId, ComponentLoaderBase *componentLoader);

    TypedComponentHandle LoadComponent(EntityHandle entity, const nlohmann::json &component);
    TypedComponentHandle CreateComponent(EntityHandle entity, ComponentTypeId typeId);

private:
    std::unordered_map<std::string, ComponentLoaderBase*> m_ComponentLoaderMap;
    std::unordered_map<ComponentTypeId, ComponentLoaderBase*> m_ComponentLoaderTypeMap;
    AssetDatabase *m_AssetDatabase;
};
