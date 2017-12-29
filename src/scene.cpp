#include "scene.hpp"

#include "assetdatabase.hpp"
#include "transform.hpp"

struct SerializedEntity
{
public:
    EntityHandle parentEntity;
    bool hasParent;
    nlohmann::json entityData;
};

void LoadSceneContent(SceneData &scene, EntityDatabase &entityDatabase, ComponentLoaderSystem &componentLoader)
{
    auto entityIterator = scene.entities.begin();

    std::vector<SerializedEntity> children;

    while (entityIterator != scene.entities.end())
    {
        SerializedEntity serializedEntity;

        serializedEntity.hasParent = false;
        serializedEntity.entityData = *entityIterator;

        children.push_back(serializedEntity);

        ++entityIterator;
    }

    while (children.size() != 0)
    {
        SerializedEntity serializedEntity = children.front();
        children.erase(children.begin());

        nlohmann::json &entity = serializedEntity.entityData;

        bool hasName = false;
        EntityHandle createdEntity;

        if (entity.count("name") != 0)
        {
            std::string name = entity["name"];
            hasName = true;

            createdEntity = entityDatabase.CreateEntity(name);
        }
        else
        {
            createdEntity = entityDatabase.CreateEntity();
        }

        if (entity.count("components") != 0)
        {
            auto components = entity["components"];
            auto componentIterator = components.begin();

            while (componentIterator != components.end())
            {
                auto &currentComponentJsonData = *componentIterator;

                TypedComponentHandle componentHandle = componentLoader.LoadComponent(createdEntity, currentComponentJsonData);

                entityDatabase.Bind(createdEntity, componentHandle.componentHandle, componentHandle.componentTypeId);

                ++componentIterator;
            }
        }

        if (serializedEntity.hasParent)
        {
            ComponentHandle parentTransformHandle;

            entityDatabase.FindComponentHandle<Transform>(serializedEntity.parentEntity, parentTransformHandle);

            Transform *entityTransform = entityDatabase.GetComponent<Transform>(createdEntity);

            entityTransform->SetParent(parentTransformHandle);
        }

        if (entity.count("entities") != 0)
        {
            auto serializedChildren = entity["entities"];
            auto childIterator = serializedChildren.begin();

            while (childIterator != serializedChildren.end())
            {
                auto &currentChild = *childIterator;

                SerializedEntity childObject;

                childObject.parentEntity = createdEntity;
                childObject.entityData = currentChild;
                childObject.hasParent = true;

                children.push_back(childObject);

                ++childIterator;
            }
        }
    }
}

EntityHandle InstantiatePrefab(Prefab &prefab, EntityDatabase &entityDatabase, ComponentLoaderSystem &componentLoader)
{
    std::vector<SerializedEntity> children;
    std::vector<EntityHandle> createdEntites;

    nlohmann::json &rootEntityData = prefab.content;

    SerializedEntity serializedEntity;

    serializedEntity.hasParent = false;
    serializedEntity.entityData = prefab.content;

    children.push_back(serializedEntity);

    while (children.size() != 0)
    {
        SerializedEntity serializedEntity = children.front();
        children.erase(children.begin());

        nlohmann::json &entity = serializedEntity.entityData;

        bool hasName = false;
        EntityHandle createdEntity;

        if (entity.count("name") != 0)
        {
            std::string name = entity["name"];
            hasName = true;

            createdEntity = entityDatabase.CreateEntity(name);
        }
        else
        {
            createdEntity = entityDatabase.CreateEntity();
        }

        createdEntites.push_back(createdEntity);

        if (entity.count("components") != 0)
        {
            auto components = entity["components"];
            auto componentIterator = components.begin();

            while (componentIterator != components.end())
            {
                auto &currentComponentJsonData = *componentIterator;

                TypedComponentHandle componentHandle = componentLoader.LoadComponent(createdEntity, currentComponentJsonData);

                entityDatabase.Bind(createdEntity, componentHandle.componentHandle, componentHandle.componentTypeId);

                ++componentIterator;
            }
        }

        if (serializedEntity.hasParent)
        {
            ComponentHandle parentTransformHandle;

            entityDatabase.FindComponentHandle<Transform>(serializedEntity.parentEntity, parentTransformHandle);

            Transform *entityTransform = entityDatabase.GetComponent<Transform>(createdEntity);

            entityTransform->SetParent(parentTransformHandle);
        }

        if (entity.count("entities") != 0)
        {
            auto serializedChildren = entity["entities"];
            auto childIterator = serializedChildren.begin();

            while (childIterator != serializedChildren.end())
            {
                auto &currentChild = *childIterator;

                SerializedEntity childObject;

                childObject.parentEntity = createdEntity;
                childObject.entityData = currentChild;
                childObject.hasParent = true;

                children.push_back(childObject);

                ++childIterator;
            }
        }
    }

    return createdEntites[0];
}
