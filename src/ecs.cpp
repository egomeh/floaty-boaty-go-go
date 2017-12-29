#include "ecs.hpp"

#include "transform.hpp"

#include <sstream>

bool ComponentBase::IsActive() const
{
    return m_Active;
}

bool ComponentBase::IsDeleted() const
{
    return m_Deleted;
}

void ComponentBase::MarkAsDeleted()
{
    m_Deleted = true;
}

EntityDatabase::EntityDatabase() : m_EntityCounter(0)
{
}

bool EntityDatabase::FindComponentHandle(EntityHandle entity, ComponentHandle &component, ComponentTypeId componentTypeId)
{
    if (m_Entities.count(entity) == 0)
    {
        return false;
    }

    auto &entityRelation = m_Entities[entity].entityRelations;

    if (entityRelation.count(componentTypeId) == 0)
    {
        return false;
    }

    if (entityRelation[componentTypeId].size() == 0)
    {
        return false;
    }

    component = entityRelation[componentTypeId][0];

    return true;
}

void EntityDatabase::FindAllComponentHandles(EntityHandle entity, std::vector<EntityHandle> &componentHandles, ComponentTypeId componentTypeId)
{
    componentHandles.clear();

    if (m_Entities.count(entity) == 0)
    {
        return;
    }

    auto &entityRelation = m_Entities[entity].entityRelations;

    if (entityRelation.count(componentTypeId) == 0)
    {
        return;
    }

    if (entityRelation[componentTypeId].size() == 0)
    {
        return;
    }

    auto &components = entityRelation[componentTypeId];

    for (const auto &component : components)
    {
        componentHandles.push_back(component);
    }
}

bool EntityDatabase::FindEntityByName(std::string name, EntityHandle &entityHandle) const
{
    bool found = false;

    for (const std::pair<EntityHandle, EntityData> &entity : m_Entities)
    {
        if (entity.second.name.compare(name) == 0)
        {
            found = true;
            entityHandle = entity.first;
        }
    }

    return found;
}

void EntityDatabase::Bind(EntityHandle entity, ComponentHandle component, ComponentTypeId componentTypeId)
{
    if (m_Entities.count(entity) == 0)
    {
        // TODO: No entity found, act 
        // This must trigger a nuclear assertion error!
        return;
    }

    EntityData &entityData = m_Entities.find(entity)->second;

    auto &entityRelations = entityData.entityRelations;

    if (entityRelations.count(componentTypeId) != 0)
    {
        entityRelations.insert(std::make_pair(componentTypeId, std::vector<ComponentHandle>()));
    }

    std::vector<ComponentHandle> &componentList = entityRelations[componentTypeId];

    componentList.push_back(component);
}

void EntityDatabase::RegisterComponentSystem(ComponentSystemBase *system, ComponentTypeId typeId)
{
    if (m_ComponentSystems.size() < (typeId + 1))
    {
        m_ComponentSystems.resize(typeId + 1);
    }

    m_ComponentSystems[typeId] = system;
}

std::vector<EntityHandle> EntityDatabase::FindEntitiesByName(const std::string & name)
{
    std::vector<EntityHandle> entitiesByName;

    for (const std::pair<EntityHandle, EntityData> &entity : m_Entities)
    {
        const EntityData &entityData = entity.second;

        if (name.compare(entityData.name) == 0)
        {
            entitiesByName.push_back(entity.first);
        }
    }

    return entitiesByName;
}

EntityHandle EntityDatabase::CreateEntity()
{
    EntityHandle newId = ++m_EntityCounter;

    std::stringstream nameStream;

    nameStream << "Entity " << newId;

    EntityData newEntityData;

    newEntityData.name = nameStream.str();

    m_Entities.insert(std::make_pair(newId, newEntityData));

    return newId;
}

EntityHandle EntityDatabase::CreateEntity(std::string name)
{
    EntityHandle newId = ++m_EntityCounter;

    EntityData newEntityData;

    newEntityData.name = name;

    m_Entities.insert(std::make_pair(newId, newEntityData));

    return newId;
}

void EntityDatabase::MarkEntityForDestruction(EntityHandle entity)
{
    if (std::find(m_DeletedEntities.begin(), m_DeletedEntities.end(), entity) == m_DeletedEntities.end())
    {
        m_DeletedEntities.push_back(entity);

        Transform *entityTransform = GetComponent<Transform>(entity);

        if (entityTransform == nullptr)
        {
            return;
        }

        const std::vector<ComponentHandle> &children = entityTransform->GetChildren();

        for (const ComponentHandle &child : children)
        {
            const ComponentHandle childHandle = child;

            const ComponentTypeId transformTypeId = GetComponentTypeID<Transform>();

            TransformSystem *transformSystem = static_cast<TransformSystem*>(m_ComponentSystems[transformTypeId]);

            Transform *childTransform = transformSystem->GetComponent(childHandle);

            MarkEntityForDestruction(childTransform->owner);
        }
    }
}

void EntityDatabase::DestroyMarkedEntities()
{
    while (m_DeletedEntities.size() > 0)
    {
        const EntityHandle entity = m_DeletedEntities.back();

        m_DeletedEntities.pop_back();

        auto it = m_Entities.find(entity);

        if (it == m_Entities.end())
        {
            return;
        }

        EntityData &entityData = it->second;

        for (const auto &entityRelation : entityData.entityRelations)
        {
            ComponentTypeId typeId = entityRelation.first;

            ComponentSystemBase *componentSystem = m_ComponentSystems[typeId];

            for (const auto &handle : entityRelation.second)
            {
                componentSystem->DestroyComponent(handle);
            }
        }
    }
}

const FrameContext &EntityDatabase::GetFrameContext() const
{
    return m_FrameContext;
}

FrameContext & EntityDatabase::GetFrameContext()
{
    return m_FrameContext;
}

void EntityDatabase::SetFrameContext(FrameContext frameContext)
{
    m_FrameContext = frameContext;
}

EntityData::EntityData()
{
}
