#include "meshasset.hpp"

#include "serialization/jsonserializer.hpp"

#include "graphics/mesh.hpp"
#include "graphics/graphicsutils.hpp"

#include <algorithm>
#include <vector>

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include "util.hpp"

Mesh *MeshAssetFactory::GetAsset(const std::string &name)
{
    if (m_LoadedAssets.count(name) != 0)
    {
        return m_LoadedAssets[name].get();
    }

    nlohmann::json &meshAsset = (*m_AssetSubMap)[name];

    std::string path = meshAsset["path"];

    MeshAssetMetaData meta;

    if (meshAsset["hasMeta"])
    {
        std::string metaDataPath = path + ".meta";

        std::string metaData = m_ResourceLoader->LoadFileAsText(metaDataPath);

        JsonSerializer serializer(metaData);

        meta.Deserialize(serializer);
    }

    std::vector<unsigned char> rawMeshData;

    m_ResourceLoader->LoadFileContent(path, rawMeshData);

    Assimp::Importer importer;

    unsigned int importFlags = aiProcess_Triangulate;

    if (!meta.flipz)
    {
        importFlags = importFlags | aiProcess_FlipWindingOrder;
    }

    NormalSource normalImportMode = StringToEnum(meta.normals, meshNormalSourceMap, NormalSource::Import);

    if (normalImportMode == NormalSource::Smooth)
    {
        importFlags |= aiProcess_GenSmoothNormals;
    }
    else if (normalImportMode == NormalSource::Flat)
    {
        importFlags |= aiProcess_GenNormals;
    }

    const aiScene *scene = importer.ReadFileFromMemory(rawMeshData.data(), rawMeshData.size(), importFlags);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::stringstream log;

        log << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;

        DebugLog(log.str());

        return nullptr;
    }

    std::vector<const aiMesh *> subMeshes;

    std::vector<const aiNode *> nodesToVisit;

    nodesToVisit.push_back(scene->mRootNode);

    while (nodesToVisit.size() > 0)
    {
        const aiNode *node = nodesToVisit.back();
        nodesToVisit.pop_back();

        const int numberOfMeshes = node->mNumMeshes;

        for (int i = 0; i < numberOfMeshes; ++i)
        {
            subMeshes.push_back(scene->mMeshes[node->mMeshes[i]]);
        }

        const int numberOfChilderen = node->mNumChildren;

        for (int i = 0; i < numberOfChilderen; ++i)
        {
            nodesToVisit.push_back(node->mChildren[i]);
        }
    }

    std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();

    std::vector<glm::vec3> vertexPositions;
    std::vector<glm::vec3> vertexNormals;
    std::vector<glm::vec2> vertexTextureCoordinates;

    std::vector<unsigned int> indices;

    indices.clear();

    for (int i = 0; i < subMeshes.size(); ++i)
    {
        unsigned int indexOffset = static_cast<unsigned int>(vertexPositions.size());

        const aiMesh *subMesh = subMeshes[i];

        const int numberOfVertecies = subMesh->mNumVertices;

        for (int vertexIndex = 0; vertexIndex < numberOfVertecies; ++vertexIndex)
        {
            const aiVector3D &position = subMesh->mVertices[vertexIndex];
            const aiVector3D &normal = subMesh->mNormals[vertexIndex];

            glm::vec3 vertexPosition(position.x, position.y, position.z);

            if (meta.flipz)
            {
                vertexPosition.z = -vertexPosition.z;
            }

            glm::vec3 vertexNormal(normal.x, normal.y, normal.z);

            if (meta.flipz)
            {
                vertexNormal.z = -vertexNormal.z;
            }

            vertexPositions.push_back(vertexPosition);
            vertexNormals.push_back(vertexNormal);

            glm::vec2 vertexUv(0.0f);

            if (subMesh->mTextureCoords[0] != nullptr)
            {
                const aiVector3D uv = subMesh->mTextureCoords[0][vertexIndex];
                vertexUv.x = uv.x;
                vertexUv.y = uv.y;
            }

            vertexTextureCoordinates.push_back(vertexUv);
        }

        const int numberOfFaces = subMesh->mNumFaces;

        for (int faceIndex = 0; faceIndex < numberOfFaces; ++faceIndex)
        {
            const aiFace &face = subMesh->mFaces[faceIndex];

            const int numberOfIndices = face.mNumIndices;

            for (int faceVertexIndex = 0; faceVertexIndex < numberOfIndices; ++faceVertexIndex)
            {
                indices.push_back(face.mIndices[faceVertexIndex] + indexOffset);
            }
        }
    }

    std::shared_ptr<BasicMeshAttribute<glm::vec3>> positionAttribute = std::make_shared<BasicMeshAttribute<glm::vec3>>(Float, 3);
    positionAttribute->SetData(vertexPositions);

    std::shared_ptr<BasicMeshAttribute<glm::vec3>> normalAttribute = std::make_shared<BasicMeshAttribute<glm::vec3>>(Float, 3);
    normalAttribute->SetData(vertexNormals);

    std::shared_ptr<BasicMeshAttribute<glm::vec2>> uvAttribute = std::make_shared<BasicMeshAttribute<glm::vec2>>(Float, 2);
    uvAttribute->SetData(vertexTextureCoordinates);

    mesh->SetAttribute(0, positionAttribute);
    mesh->SetAttribute(1, normalAttribute);
    mesh->SetAttribute(2, uvAttribute);
    mesh->SetIndecies(indices);

    mesh->GenerateDeviceBuffers();

    m_LoadedAssets.insert(std::make_pair(name, mesh));

    return mesh.get();
}
