#include "transform.hpp"

#include "glm/gtc/matrix_transform.hpp"

Transform::Transform(TransformSystem *transformSystem, ComponentHandle id) : m_TransformSystem(transformSystem), m_OwnId(id)
{
    SetDirty();
    m_IsRoot = true;
}

glm::mat4 Transform::GetTransformMatrix()
{
    if (m_Dirty)
    {
        glm::mat4 localObjectToWorld(1.);

        localObjectToWorld = glm::translate(localObjectToWorld, m_LocalPosition);
        localObjectToWorld *= glm::toMat4(m_LocalRotation);


        if (m_IsRoot)
        {
            m_ObjectToWorld = localObjectToWorld;
        }
        else
        {
            Transform *parentTransform = m_TransformSystem->GetComponent(m_Parent);

            glm::mat4 parentObjectToWorld = parentTransform->GetTransformMatrix();

            m_ObjectToWorld = parentObjectToWorld * localObjectToWorld;
        }

        m_Dirty = false;
    }

    m_WorldToObject = glm::inverse(m_ObjectToWorld);

    return m_ObjectToWorld;
}

glm::vec3 Transform::GetLocalForwards()
{
    return m_LocalRotation * glm::vec3(0., 0., 1.);
}

glm::vec3 Transform::GetLocalUp()
{
    return m_LocalRotation * glm::vec3(0., 1., 0.);
}

glm::vec3 Transform::GetLocalRight()
{
    return m_LocalRotation * glm::vec3(1., 0., 0.);
}

glm::vec3 Transform::GetLocalPosition()
{
    return m_LocalPosition;
}

glm::quat Transform::GetLocalRotation()
{
    return m_LocalRotation;
}

glm::vec3 Transform::GetLocalEulerAngles()
{
    return glm::degrees(glm::eulerAngles(m_LocalRotation));
}

inline void Transform::SetDirty()
{
    m_Dirty = true;

    for (const ComponentHandle childHandle : m_Children)
    {
        Transform *child = m_TransformSystem->GetComponent(childHandle);

        child->SetDirty();
    }
}

void Transform::SetLocalPosition(glm::vec3 position)
{
    m_LocalPosition = position;

    SetDirty();
}

void Transform::SetLocalRotation(glm::quat rotation)
{
    m_LocalRotation = rotation;

    SetDirty();
}

void Transform::SetLocalEulerAngles(glm::vec3 eulerAngles)
{
    m_LocalRotation = glm::quat(glm::radians(eulerAngles));

    SetDirty();
}

void Transform::SetAsRootNode()
{
    if (m_IsRoot)
    {
        return;
    }

    Transform *oldParentTransform = m_TransformSystem->GetComponent(m_Parent);

    oldParentTransform->RemoveChild(m_OwnId);
    oldParentTransform->SetDirty();

    m_IsRoot = true;

    SetDirty();
}

void Transform::SetParent(ComponentHandle parent)
{
    ASSERT(parent != m_OwnId, "Parent To transform cannot be itself");

    if (!m_IsRoot)
    {
        Transform *oldParentTransform = m_TransformSystem->GetComponent(m_Parent);

        oldParentTransform->RemoveChild(m_OwnId);
        oldParentTransform->SetDirty();
    }

    Transform *parentTransform = m_TransformSystem->GetComponent(parent);

    parentTransform->AddChild(m_OwnId);
    parentTransform->SetDirty();

    m_IsRoot = false;

    m_Parent = parent;

    SetDirty();
}

void Transform::AddChild(ComponentHandle child)
{
    ASSERT(child != m_OwnId, "Child of transform cannot be itself");

    if (std::find(m_Children.begin(), m_Children.end(), child) != m_Children.end())
    {
        return;
    }

    m_Children.push_back(child);
}

void Transform::RemoveChild(ComponentHandle child)
{
    auto childPosition = std::find(m_Children.begin(), m_Children.end(), child);

    if (childPosition == m_Children.end())
    {
        return;
    }

    m_Children.erase(childPosition);
}

const std::vector<ComponentHandle> &Transform::GetChildren() const
{
    return m_Children;
}

void Transform::SetTransformSystem(TransformSystem *transformSystem)
{
    m_TransformSystem = transformSystem;
}

bool TransformSystem::Create(ComponentHandle & handle)
{
    std::size_t id = 0;
    bool insertIntoFreeSlot = false;

    if (m_FreeComponentSlots.size() != 0)
    {
        id = m_FreeComponentSlots.back();

        m_FreeComponentSlots.pop_back();

        insertIntoFreeSlot = true;
    }
    else
    {
        id = m_Transforms.size();

        m_ComponentMapping.push_back(static_cast<int>(id));
    }

    Transform newComponent(this, id);

    if (insertIntoFreeSlot)
    {
        m_Transforms[m_ComponentMapping[id]] = newComponent;
    }
    else
    {
        m_Transforms.push_back(newComponent);
    }

    handle = id;

    return true;
}

void TransformSystem::Update()
{

}

std::vector<Transform>& TransformSystem::GetComponents()
{
    return m_Transforms;
}

Transform *TransformSystem::GetComponent(ComponentHandle handle)
{
    if (handle > m_ComponentMapping.size())
    {
        return nullptr;
    }

    Transform &component = m_Transforms[m_ComponentMapping[handle]];

    return &component;
}

void TransformSystem::DestroyComponent(ComponentHandle handle)
{
    if (handle < m_ComponentMapping.size())
    {
        Transform &component = m_Transforms[m_ComponentMapping[handle]];
        if (component.IsDeleted())
        {
            return;
        }
        else
        {
            component.MarkAsDeleted();
            m_FreeComponentSlots.push_back(handle);
        }
    }
}
