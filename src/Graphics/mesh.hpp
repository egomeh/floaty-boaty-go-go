#pragma once

#include "graphicsutils.hpp"
#include "graphics.hpp"

#include "serialization/serialization.hpp"

class MeshAssetMetaData
{
public:
    MeshAssetMetaData();

    template<typename SerializationType>
    void Deserialize(SerializationType serializer)
    {
        DESERIALIZE(flipz, serializer);
        DESERIALIZE(normals, serializer);
    }

    bool flipz;
    std::string normals;
};

enum AttributeType
{
    Float,
    Integer
};

class MeshAttribute
{
public:
    virtual const void *GetData() const = 0;
    virtual std::size_t GetElementSize() const = 0;
    virtual std::size_t GetQuantity() const = 0;
    virtual std::size_t GetValuesPerElement() const = 0;
    virtual AttributeType GetType() const = 0;

    static GLenum AttributeTypeTpoGLType(AttributeType type);
};

template<typename T>
class BasicMeshAttribute : public MeshAttribute
{
public:
    BasicMeshAttribute(AttributeType type, std::size_t valuesPerElement) : m_Type(type), m_ValuesPerElement(valuesPerElement) {}

    virtual const void *GetData() const override { return m_Data.data(); }
    virtual std::size_t GetElementSize() const override { return sizeof(T); }
    virtual std::size_t GetQuantity() const override { return m_Data.size(); }
    virtual std::size_t GetValuesPerElement() const { return m_ValuesPerElement; };
    virtual AttributeType GetType() const { return m_Type; };

    void SetData(std::vector<T> data) { m_Data = data; };
    void AddData(T element) { m_Data.push_back(element); };
    void Clear() { m_Data.clear(); };

private:
    AttributeType m_Type;
    std::size_t m_ValuesPerElement;
    std::size_t m_ElementSize;
    std::vector<T> m_Data;
};

class Mesh
{
public:
    Mesh();
    ~Mesh();

    void SetAttribute(std::size_t channel, std::shared_ptr<MeshAttribute> attribute);
    void SetIndecies(std::vector<unsigned int> indecies);
    void ClearIndecies();
    bool HasDevicBuffers() const;

    void Bind();

    void GenerateDeviceBuffers();
    void DeleteDeviceBuffers();

    std::size_t GetRenderCount() const;
    void Draw() const;

private:
    std::map<std::size_t, std::shared_ptr<MeshAttribute>> m_Attributes;
    std::vector<unsigned int> m_Indecies;

    bool m_HasDeviceBuffers;
    bool m_HasIndexBuffer;
    GLuint m_VertexArrayObject;
    std::vector<GLuint> m_VertexBufferObjects;
    GLuint m_IndexBufferObject;
};
