#include "logic.hpp"

#include "logicfactories.hpp"

void LogicSystem::Initialize()
{
    RegisterLogicFactories(*this);
    m_HandleCounter = 0;
    m_ComponentsNotYetStarted.clear();
}

void LogicSystem::Update()
{
    for (auto &logicComponent : m_Components)
    {
        logicComponent.second.pointer->Update();
    }
}

void LogicSystem::LateUpdate()
{
    for (auto &logicComponent : m_Components)
    {
        logicComponent.second.pointer->LateUpdate();
    }
}

void LogicSystem::ResolveCollisions(const std::vector<Collision> &collisions)
{
    for (const Collision &collision : collisions)
    {
        for (const auto &logicComponent : m_Components)
        {
            const EntityHandle componentOwner = logicComponent.second.pointer->owner;

            if (componentOwner == collision.body0)
            {
                logicComponent.second.pointer->OnCollision(collision.body1);
            }
            else if (componentOwner == collision.body1)
            {
                logicComponent.second.pointer->OnCollision(collision.body0);
            }
        }
    }
}

TypedComponentHandle LogicSystem::LoadComponent(const nlohmann::json & serializedComponent)
{
    TypedComponentHandle handle {0, 0};

    const std::string &componentType = serializedComponent["type"];

    for (const auto &factoryMapping : m_FactoryNameMap)
    {
        const std::string &factoryName = factoryMapping.first;

        if (factoryName.compare(componentType) == 0)
        {
            LogicComponentFactoryBase *factory = factoryMapping.second;

            ComponentTypeId typeId = factory->GetFactoryTypeId();
            ComponentHandle componentHandle = ++m_HandleCounter;

            LogicComponentPointer createdComponent;

            createdComponent.typeId = typeId;
            createdComponent.pointer = std::shared_ptr<LogicComponent>(factory->CreatePointer(serializedComponent));
            createdComponent.pointer->SetEntityDatabase(m_EntityDatabase);
            createdComponent.pointer->SetEntityComponentLoader(m_ComponentLoaderSystem);
            createdComponent.pointer->SetAssetDatabase(m_AssetDatabase);

            handle.componentHandle = componentHandle;
            handle.componentTypeId = typeId;

            m_ComponentsNotYetStarted.insert(std::make_pair(componentHandle, createdComponent));
        }
    }

    return handle;
}

TypedComponentHandle LogicSystem::LoadComponent(ComponentTypeId typeId)
{
    TypedComponentHandle handle{0, 0};

    for (auto &factoryMapping : m_FactoryTypeMap)
    {
        const ComponentTypeId &factoryType = factoryMapping.first;

        if (factoryType == typeId)
        {
            LogicComponentFactoryBase *factory = factoryMapping.second;

            ComponentHandle componentHandle = ++m_HandleCounter;

            LogicComponentPointer createdComponent;

            createdComponent.typeId = typeId;
            createdComponent.pointer = std::shared_ptr<LogicComponent>(factory->CreatePointer());
            createdComponent.pointer->SetEntityDatabase(m_EntityDatabase);
            createdComponent.pointer->SetEntityComponentLoader(m_ComponentLoaderSystem);
            createdComponent.pointer->SetAssetDatabase(m_AssetDatabase);

            handle.componentHandle = componentHandle;
            handle.componentTypeId = typeId;

            m_ComponentsNotYetStarted.insert(std::make_pair(componentHandle, createdComponent));
        }
    }

    return handle;
}

ComponentBase *LogicSystem::GetAbstractComponent(ComponentHandle handle)
{
    auto iterator = m_Components.find(handle);

    if (iterator == m_Components.end())
    {
        auto iteratorNotStartedQueue = m_ComponentsNotYetStarted.find(handle);

        if (iteratorNotStartedQueue == m_ComponentsNotYetStarted.end())
        {
            return nullptr;
        }
        else
        {
            LogicComponentPointer &component = iteratorNotStartedQueue->second;

            return iteratorNotStartedQueue->second.pointer.get();
        }
    }

    LogicComponentPointer &component = iterator->second;

    return iterator->second.pointer.get();
}

LogicComponent *LogicSystem::GetComponent(ComponentHandle handle)
{
    auto iterator = m_Components.find(handle);

    if (iterator == m_Components.end())
    {
        auto iteratorNotStartedQueue = m_ComponentsNotYetStarted.find(handle);

        if (iteratorNotStartedQueue == m_ComponentsNotYetStarted.end())
        {
            return nullptr;
        }
        else
        {
            LogicComponentPointer &component = iteratorNotStartedQueue->second;

            return iteratorNotStartedQueue->second.pointer.get();
        }
    }

    return iterator->second.pointer.get();
}

void LogicSystem::DestroyComponent(ComponentHandle handle)
{
    auto it = m_Components.find(handle);

    if (it != m_Components.end())
    {
        it->second.pointer->OnDestroy();

        m_Components.erase(handle);
    }
}

void LogicSystem::RegisterWithComponentLoaderSystem(ComponentLoaderSystem &componentLoaderSystem)
{
    for (const auto &factoryMapping : m_FactoryNameMap)
    {
        const std::string &factoryName = factoryMapping.first;
        const ComponentTypeId factoryType = factoryMapping.second->GetFactoryTypeId();
        componentLoaderSystem.RegisterComponentLoader(factoryName, factoryType, this);
    }

    m_ComponentLoaderSystem = &componentLoaderSystem;
}

void LogicSystem::RegisterComponentSystems(EntityDatabase *entityDatabase)
{
    for (const auto &factoryMapping : m_FactoryTypeMap)
    {
        ComponentTypeId typeId = factoryMapping.first;

        entityDatabase->RegisterComponentSystem(this, typeId);
    }
}

void LogicSystem::CallStartOnLoadedComponents()
{
    for (const auto &logicComponent: m_ComponentsNotYetStarted)
    {
        logicComponent.second.pointer->Start();

        m_Components.insert(logicComponent);
    }

    m_ComponentsNotYetStarted.clear();
}

void LogicComponent::SetEntityDatabase(EntityDatabase *entityDatabase)
{
    m_EntityDatabase = entityDatabase;
}

void LogicComponent::SetEntityComponentLoader(ComponentLoaderSystem *componentLoader)
{
    m_ComponentLoaderSystem = componentLoader;
}

void LogicComponent::SetAssetDatabase(AssetDatabase *assetDatabase)
{
    m_AssetDatabase = assetDatabase;
}

const FrameContext &LogicComponent::GetFrameContext() const
{
    return m_EntityDatabase->GetFrameContext();
}

EntityHandle LogicComponent::Instantiate(Prefab *prefab)
{
    return InstantiatePrefab(*prefab, *m_EntityDatabase, *m_ComponentLoaderSystem);
}
