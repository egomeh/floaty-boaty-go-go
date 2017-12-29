#pragma once

#include "serialization/serialization.hpp"

#include <vector>

#include "ecs.hpp"

#define GLM_ENABLE_EXPERIMENTAL

#include "glm/gtx/quaternion.hpp"

class TransformSystem;

class Transform : public ComponentBase
{
public:
    Transform(TransformSystem *transformSystem, ComponentHandle componentId);

    template<typename SerializationType>
    void Deserialize(SerializationType serializer)
    {
        glm::vec3 position;
        glm::vec3 rotation;
        glm::vec3 scale;

        DESERIALIZE(position, serializer);
        DESERIALIZE(rotation, serializer);
        DESERIALIZE(scale, serializer);

        m_LocalPosition = position;
        m_LocalScale = scale;
        m_LocalRotation = glm::quat(glm::radians(rotation));

        SetDirty();

        m_IsRoot = true;

        m_Parent = 0;
    }

    glm::mat4 GetTransformMatrix();

    glm::vec3 GetLocalForwards();
    glm::vec3 GetLocalUp();
    glm::vec3 GetLocalRight();

    glm::vec3 GetLocalPosition();
    glm::quat GetLocalRotation();
    glm::vec3 GetLocalEulerAngles();

    inline void SetDirty();

    void SetLocalPosition(glm::vec3 position);
    void SetLocalRotation(glm::quat rotation);
    void SetLocalEulerAngles(glm::vec3 eulerAngles);

    void SetAsRootNode();
    void SetParent(ComponentHandle parent);
    void AddChild(ComponentHandle child);
    void RemoveChild(ComponentHandle child);

    const std::vector<ComponentHandle> &GetChildren() const;

    void SetTransformSystem(TransformSystem * transformSystem);

private:
    glm::vec3 m_LocalPosition;
    glm::quat m_LocalRotation;
    glm::vec3 m_LocalScale;

    glm::mat4 m_ObjectToWorld;
    glm::mat4 m_WorldToObject;

    bool m_Dirty;
    bool m_IsRoot;

    ComponentHandle m_Parent;
    ComponentHandle m_OwnId;
    
    std::vector<ComponentHandle> m_Children;

    TransformSystem *m_TransformSystem;
};

class TransformSystem : public ComponentSystem<Transform>
{
public:
    bool Create(ComponentHandle &handle);

    void Update();

    std::vector<Transform> &GetComponents();
    Transform *GetComponent(ComponentHandle handle) override;

    void DestroyComponent(ComponentHandle handle) override;

private:
    std::vector<Transform> m_Transforms;

    std::vector<std::size_t> m_FreeComponentSlots;
    std::vector<int> m_ComponentMapping;
};
