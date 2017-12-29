#pragma once

#include "mesh.hpp"

#include "glm/glm.hpp"
#include "graphicsutils.hpp"

#include <algorithm>

Mesh::Mesh() : m_HasDeviceBuffers(false), m_IndexBufferObject(0), m_VertexArrayObject(0)
{
}

Mesh::~Mesh()
{
    DeleteDeviceBuffers();
}

void Mesh::SetAttribute(std::size_t channel, std::shared_ptr<MeshAttribute> attribute)
{
    m_Attributes.insert(std::make_pair(channel, attribute));
}

void Mesh::SetIndecies(std::vector<unsigned int> indecies)
{
    m_Indecies = indecies;
}

void Mesh::ClearIndecies()
{
    if (m_IndexBufferObject != 0)
    {
        glDeleteBuffers(1, &m_IndexBufferObject);
        GL_ERROR_CHECK();
    }

    m_IndexBufferObject = 0;

    m_Indecies.clear();
}

bool Mesh::HasDevicBuffers() const
{
    return m_HasDeviceBuffers;
}

void Mesh::Bind()
{
    glBindVertexArray(m_VertexArrayObject);
    GL_ERROR_CHECK();

    if (m_Indecies.size())
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBufferObject);
        GL_ERROR_CHECK();
    }
}

void Mesh::GenerateDeviceBuffers()
{
    DeleteDeviceBuffers();

    glGenVertexArrays(1, &m_VertexArrayObject);
    GL_ERROR_CHECK();

    glBindVertexArray(m_VertexArrayObject);
    GL_ERROR_CHECK();

    if (m_Indecies.size())
    {
        glGenBuffers(1, &m_IndexBufferObject);
        GL_ERROR_CHECK();

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBufferObject);
        GL_ERROR_CHECK();

        glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Indecies.size() * sizeof(unsigned int), m_Indecies.data(), GL_STATIC_DRAW);
        GL_ERROR_CHECK();
    }

    for (const auto &attributeMapping : m_Attributes)
    {
        GLuint attribute;

        glGenBuffers(1, &attribute);
        GL_ERROR_CHECK();

        glBindBuffer(GL_ARRAY_BUFFER, attribute);
        GL_ERROR_CHECK();

        std::shared_ptr<MeshAttribute> meshAttribute = attributeMapping.second;

        glBufferData(GL_ARRAY_BUFFER, meshAttribute->GetQuantity() * meshAttribute->GetElementSize(), meshAttribute->GetData(), GL_STATIC_DRAW);
        GL_ERROR_CHECK();

        GLsizei attributeIndx = static_cast<GLsizei>(attributeMapping.first);

        glEnableVertexAttribArray(attributeIndx);
        GL_ERROR_CHECK();

        GLenum type = MeshAttribute::AttributeTypeTpoGLType(attributeMapping.second->GetType());
        GLsizei attributeIndex = static_cast<GLsizei>(attributeMapping.first);
        GLsizei attributeSize = static_cast<GLsizei>(attributeMapping.second->GetValuesPerElement());

        glVertexAttribPointer(attributeIndex, attributeSize, type, GL_FALSE, 0, NULL);
        GL_ERROR_CHECK();

        m_VertexBufferObjects.push_back(attribute);
    }

    m_HasDeviceBuffers = true;
}


void Mesh::DeleteDeviceBuffers()
{
    std::remove_if(m_VertexBufferObjects.begin(), m_VertexBufferObjects.end(), [](GLuint id) { return id == 0; });

    if (m_VertexBufferObjects.size() > 0)
    {
        glDeleteBuffers(static_cast<GLsizei>(m_VertexBufferObjects.size()), m_VertexBufferObjects.data());
        GL_ERROR_CHECK();

        m_VertexBufferObjects.clear();
    }

    if (m_IndexBufferObject != 0)
    {
        glDeleteBuffers(1, &m_IndexBufferObject);
        GL_ERROR_CHECK();
    }

    if (m_VertexArrayObject != 0)
    {
        glDeleteVertexArrays(1, &m_VertexArrayObject);
        GL_ERROR_CHECK();
    }

    m_IndexBufferObject = 0;
    m_VertexArrayObject = 0;

    m_HasDeviceBuffers = false;
}

std::size_t Mesh::GetRenderCount() const
{
    return m_Indecies.size();
}

void Mesh::Draw() const
{
    const GLsizei count = static_cast<GLsizei>(m_Indecies.size());

    glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, (void*)0);
    GL_ERROR_CHECK();
}


GLenum MeshAttribute::AttributeTypeTpoGLType(AttributeType type)
{
    switch (type)
    {
        case Float:
        {
            return GL_FLOAT;
        }
        case Integer:
        {
            return GL_INT;
        }
        default:
        {
            return GL_FLOAT;
        }
    }
}

MeshAssetMetaData::MeshAssetMetaData()
{
    flipz = false;
}
