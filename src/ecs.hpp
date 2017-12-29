#pragma once

#include <iostream>
#include <unordered_map>

#include "util.hpp"

typedef std::size_t EntityHandle;
typedef std::size_t ComponentHandle;
typedef std::size_t ComponentTypeId;

class ComponentBase
{
public:
    bool IsActive() const;
    bool IsDeleted() const;

    void MarkAsDeleted();

    EntityHandle owner;

protected:
    bool m_Active;
    bool m_Deleted;
};

class ComponentSystemBase
{
public:
    virtual void DestroyComponent(ComponentHandle) = 0;
};

template<typename T>
class ComponentSystem : public ComponentSystemBase
{
public:
    virtual T *GetComponent(ComponentHandle handle) = 0;
    virtual void DestroyComponent(ComponentHandle) = 0;
};

struct TypedComponentHandle
{
public:
    ComponentHandle componentHandle;
    ComponentTypeId componentTypeId;
};

class EntityData
{
public:
    EntityData();

    std::string name;

    std::unordered_map<std::size_t, std::vector<ComponentHandle>> entityRelations;
};

class EntityDatabase
{
public:
    EntityDatabase();

    template<typename T>
    void Bind(EntityHandle entity, ComponentHandle componet)
    {
        std::size_t componentType = GetComponentTypeID<T>();

        Bind(entity, component, componentType);
    }

    void Bind(EntityHandle entity, ComponentHandle component, ComponentTypeId componentTypeId);

    template<typename T>
    bool FindComponentHandle(EntityHandle entity, ComponentHandle &component)
    {
        ComponentTypeId typeId = GetComponentTypeID<T>();

        return FindComponentHandle(entity, component, typeId);
    }

    bool FindComponentHandle(EntityHandle entity, ComponentHandle &component, ComponentTypeId componentTypeId);

    void FindAllComponentHandles(EntityHandle entity, std::vector<ComponentHandle> &handels, ComponentTypeId typeId);

    bool FindEntityByName(std::string name, EntityHandle &entityHandle) const;

    template<typename T>
    void RegisterComponentSystem(ComponentSystem<T> *system)
    {
        ComponentTypeId typeId = GetComponentTypeID<T>();

        RegisterComponentSystem(system, typeId);
    }

    void RegisterComponentSystem(ComponentSystemBase *system, ComponentTypeId typeId);

    template<typename T>
    T *GetComponent(EntityHandle entity)
    {
        ComponentTypeId componentType = GetComponentTypeID<T>();

        ComponentHandle componentHandle;

        if (!FindComponentHandle(entity, componentHandle, componentType))
        {
            return nullptr;
        }

        if (componentType > m_ComponentSystems.size() - 1)
        {
            return nullptr;
        }

        ComponentSystemBase *abstractComponentSystem = m_ComponentSystems[componentType];

        ComponentSystem<T> *typedComponentSystem = static_cast<ComponentSystem<T>*>(abstractComponentSystem);

        return typedComponentSystem->GetComponent(componentHandle);
    }

    template<typename T>
    void GetComponents(EntityHandle entity, std::vector<T*> &components)
    {
        components.clear();

        ComponentTypeId componentType = GetComponentTypeID<T>();

        if (componentType > m_ComponentSystems.size() - 1)
        {
            return;
        }

        ComponentSystemBase *abstractComponentSystem = m_ComponentSystems[componentType];

        ComponentSystem<T> *typedComponentSystem = static_cast<ComponentSystem<T>*>(abstractComponentSystem);

        std::vector<ComponentHandle> componentHandles;

        FindAllComponentHandles(entity, componentHandles, componentType);

        for (auto it = componentHandles.begin(); it != componentHandles.end(); ++it)
        {
            T *component = typedComponentSystem->GetComponent(*it);

            components.push_back(component);
        }
    }

    template<typename T>
    T *GetComponentByHandle(EntityHandle entity, TypedComponentHandle handle)
    {
        if (handle.componentTypeId > m_ComponentSystems.size() - 1)
        {
            return nullptr;
        }

        ComponentSystemBase *abstractComponentSystem = m_ComponentSystems[handle.componentTypeId];

        ComponentSystem<T> *typedComponentSystem = static_cast<ComponentSystem<T>*>(abstractComponentSystem);

        return typedComponentSystem->GetComponent(handle.componentHandle);
    }

    std::vector<EntityHandle> FindEntitiesByName(const std::string &name);

    EntityHandle CreateEntity();
    EntityHandle CreateEntity(std::string name);

    void MarkEntityForDestruction(EntityHandle entity);
    void DestroyMarkedEntities();

    const FrameContext &GetFrameContext() const;
    FrameContext &GetFrameContext();

    void SetFrameContext(FrameContext frameContext);

private:
    std::size_t m_EntityCounter;

    std::vector<EntityHandle> m_DeletedEntities;
    std::unordered_map<EntityHandle, EntityData> m_Entities;
    std::vector<ComponentSystemBase*> m_ComponentSystems;

    FrameContext m_FrameContext;
};
