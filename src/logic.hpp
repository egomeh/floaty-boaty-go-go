#pragma once

#include <unordered_map>

#include "ecs.hpp"
#include "assetdatabase.hpp"
#include "transform.hpp"
#include "scene.hpp"
#include "Graphics/texture.hpp"

#include "Physics/collision.hpp"

class LogicComponent : public ComponentBase
{
public:
    void SetEntityDatabase(EntityDatabase *entityDatabse);
    void SetEntityComponentLoader(ComponentLoaderSystem *componentLoader);
    void SetAssetDatabase(AssetDatabase *assetDatabase);

    const FrameContext &GetFrameContext() const;

    template<typename T>
    T *GetComponent()
    {
        return m_EntityDatabase->GetComponent<T>(owner);
    }

    template<typename T>
    T *AddComponent()
    {
        const ComponentTypeId typeId = GetComponentTypeID<T>();

        TypedComponentHandle typedHandle = m_ComponentLoaderSystem->CreateComponent(owner, typeId);

        m_EntityDatabase->Bind(owner, typedHandle.componentHandle, typeId);

        return m_EntityDatabase->GetComponentByHandle<T>(owner, typedHandle);
    }

    EntityHandle Instantiate(Prefab *prefab);

    virtual void Update() {};
    virtual void LateUpdate() {};
    virtual void OnCollision(EntityHandle other) {};
    virtual void Start() {};
    virtual void OnDestroy() {};
    virtual void OnRenderImage(Texture *source, RenderTexture *target) {}

protected:
    EntityDatabase *m_EntityDatabase;
    ComponentLoaderSystem *m_ComponentLoaderSystem;
    AssetDatabase *m_AssetDatabase;
};

struct LogicComponentPointer
{   
public:
    ComponentTypeId typeId;

    std::shared_ptr<LogicComponent> pointer;
};

class LogicComponentFactoryBase
{
public:
    virtual LogicComponent *CreatePointer() = 0;
    virtual LogicComponent *CreatePointer(const nlohmann::json &serializedComponent) = 0;
    virtual ComponentTypeId GetFactoryTypeId() = 0;
};

template<typename T>
class LogicComponentFactory : public LogicComponentFactoryBase
{
public:
    LogicComponentFactory<T>()
    {
        static_assert(std::is_base_of<LogicComponent, T>::value, "T must be a logic component!");
    }

    ComponentTypeId GetFactoryTypeId() override
    {
        return GetComponentTypeID<T>();
    }

    virtual LogicComponent *CreatePointer() = 0;
    virtual LogicComponent *CreatePointer(const nlohmann::json &serializedComponent) = 0;
};

class LogicSystem : public ComponentLoader<LogicComponent>, public ComponentSystem<LogicComponent>
{
public:
    void Initialize();
    void Update();
    void LateUpdate();
    void ResolveCollisions(const std::vector<Collision> &collisions);

    TypedComponentHandle LoadComponent(const nlohmann::json &serializedComponent) override;
    TypedComponentHandle LoadComponent(ComponentTypeId typeId) override;
    ComponentBase *GetAbstractComponent(ComponentHandle handle) override;

    LogicComponent *GetComponent(ComponentHandle handle) override;

    void DestroyComponent(ComponentHandle handle) override;

    template<typename T>
    void RegisterFactory(std::string componentName, LogicComponentFactoryBase *factory)
    {
        std::size_t typeId = GetComponentTypeID<T>();

        m_FactoryNameMap.insert(std::make_pair(componentName, factory));
        m_FactoryTypeMap.insert(std::make_pair(typeId, factory));
    }

    void RegisterWithComponentLoaderSystem(ComponentLoaderSystem &componentLoaderSystem);
    void RegisterComponentSystems(EntityDatabase *entityDatabase);

    void CallStartOnLoadedComponents();

    EntityDatabase *m_EntityDatabase;
    AssetDatabase *m_AssetDatabase;
    ComponentLoaderSystem *m_ComponentLoaderSystem;
    TransformSystem *m_TransformSystem;

private:
    ComponentHandle m_HandleCounter;

    std::unordered_map<std::string, LogicComponentFactoryBase*> m_FactoryNameMap;
    std::unordered_map<std::size_t, LogicComponentFactoryBase*> m_FactoryTypeMap;

    std::unordered_map<ComponentHandle, LogicComponentPointer> m_Components;

    std::unordered_map<ComponentHandle, LogicComponentPointer> m_ComponentsNotYetStarted;
};
