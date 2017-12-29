#include "assetdatabase.hpp"

AssetDatabase::AssetDatabase()
{
}

void AssetDatabase::SetAssetMapping(std::string mappingString)
{
    m_AssetMapping = nlohmann::json::parse(mappingString);
}

void AssetDatabase::SetReesourceLoader(ResourceLoader resourceLoader)
{
    m_ResourceLoader = resourceLoader;
}

AssetFactoryBase *AssetDatabase::GetAbstractAssetFactory(std::size_t typeId)
{
    if (m_AssetTypeMap.count(typeId) == 0)
    {
        return nullptr;
    }

    return m_AssetTypeMap[typeId];
}

nlohmann::json AssetDatabase::GetAssetSubMap(std::string subMap)
{
    return m_AssetMapping[subMap];
}

void ComponentLoaderSystem::SetAssetDatabase(AssetDatabase *assetDatabase)
{
    m_AssetDatabase = assetDatabase;
}

void ComponentLoaderSystem::RegisterComponentLoader(std::string componentType, ComponentTypeId typeId, ComponentLoaderBase *componentLoader)
{
    componentLoader->SetAsseDatabase(m_AssetDatabase);
    m_ComponentLoaderMap.insert(std::make_pair(componentType, componentLoader));
    m_ComponentLoaderTypeMap.insert(std::make_pair(typeId, componentLoader));
}

TypedComponentHandle ComponentLoaderSystem::LoadComponent(EntityHandle entity, const nlohmann::json &component)
{
    std::string componentType = component["type"];

    auto loaderIterator = m_ComponentLoaderMap.find(componentType);

    if (loaderIterator == m_ComponentLoaderMap.end())
    {
#ifdef _DEBUG
        std::stringstream error;

        error << "Could not find loader for component of type `";
        error << componentType;
        error << "`";

        throw std::exception(error.str().c_str());
#endif

        TypedComponentHandle handle;

        handle.componentHandle = 0;
        handle.componentTypeId = 0;

        return handle;
    }

    TypedComponentHandle handle = loaderIterator->second->LoadComponent(component);

    auto &baseComponent = *(loaderIterator->second->GetAbstractComponent(handle.componentHandle));

    baseComponent.owner = entity;

    return handle;
}

TypedComponentHandle ComponentLoaderSystem::CreateComponent(EntityHandle entity, ComponentTypeId typeId)
{
    auto loaderIterator = m_ComponentLoaderTypeMap.find(typeId);

    if (loaderIterator == m_ComponentLoaderTypeMap.end())
    {
#ifdef _DEBUG
        std::stringstream error;

        error << "Could not find loader for component of type ID `";
        error << typeId;
        error << "`";

        throw std::exception(error.str().c_str());
#endif

        TypedComponentHandle handle;

        handle.componentHandle = 0;
        handle.componentTypeId = 0;

        return handle;
    }

    TypedComponentHandle handle = loaderIterator->second->LoadComponent(typeId);

    auto &baseComponent = *(loaderIterator->second->GetAbstractComponent(handle.componentHandle));

    baseComponent.owner = entity;

    return handle;
}

void ComponentLoaderBase::SetAsseDatabase(AssetDatabase *assetDatabase)
{
    m_AssetDatabase = assetDatabase;
}
