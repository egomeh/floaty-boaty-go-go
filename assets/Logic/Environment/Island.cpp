#include "Island.hpp"

#include "Graphics/meshrenderer.hpp"

#include <random>

#define _USE_MATH_DEFINES

#include <math.h>
#include "glm/gtc/noise.hpp"

float OctavePerlin(float x, float y, float z, int octaves, float persistence)
{
    float total = 0;
    float frequency = 1;
    float amplitude = 1;
    float maxValue = 0;
    for (int i = 0; i < octaves; i++)
    {
        total += glm::perlin(glm::vec3(x * frequency, y * frequency, z * frequency)) * amplitude;

        maxValue += amplitude;

        amplitude *= persistence;
        frequency *= 2;
    }

    return total;
}

float IslandHeightFunction(float x, float y)
{
    float fromCenter = glm::length(glm::vec2(x, y));

    float height = 2.0f - std::exp(.2f * (fromCenter - 20.0f));

    height += OctavePerlin(x * .05f, y * .05f, 0.75f, 4, 0.5f);

    return std::max(-1.0f, height);
}

void Island::Update()
{
}

void Island::Start()
{
    m_Material = m_AssetDatabase->RequestAsset<Material>(material);

    std::mt19937 rng;
    rng.seed(std::random_device()());
    std::uniform_real_distribution<float> distribution(0.0f, 1.0f);

    std::normal_distribution<float> normalDistribution(0.0f, 1.0f);

    std::vector<Vector2<float>> vertecies;

    for (int i = 0; i < 1000; ++i)
    {
        float angle = static_cast<float>(M_PI) * 2.0f * distribution(rng);

        // Distribute along the island edge
        float range = std::sqrt(distribution(rng)) * 30.0f;

        // Normalize according to disk distribution
        // range = std::sqrt(range);

        float x = std::cos(angle) * range;
        float y = std::sin(angle) * range;

        Vector2<float> vertex(x, y);

        vertecies.push_back(vertex);
    }

    Delaunay<float> delaunay;

    std::vector<Triangle<float>> triangles = delaunay.triangulate(vertecies);

    auto positionAttribute = std::make_shared<BasicMeshAttribute<glm::vec3>>(Float, 3);
    auto normalAttribute = std::make_shared<BasicMeshAttribute<glm::vec3>>(Float, 3);
    auto uvAttribute = std::make_shared<BasicMeshAttribute<glm::vec2>>(Float, 2);

    std::vector<unsigned int> indices;

    for (const Triangle<float> &triangle : triangles)
    {
        const float epsilon = 0.01f;

        float height1 = IslandHeightFunction(triangle.p1.x, triangle.p1.y);
        float height1dx = IslandHeightFunction(triangle.p1.x + epsilon, triangle.p1.y);
        float height1dy = IslandHeightFunction(triangle.p1.x, triangle.p1.y + epsilon);

        glm::vec3 vertex1 = glm::vec3(triangle.p1.x, height1, triangle.p1.y);
        glm::vec3 normal1 = GetNormalFromHeightSamples(height1, height1dx, height1dy, epsilon);
        glm::vec2 uv1 = glm::vec2(triangle.p1.x, triangle.p1.y) * 0.25f;

        float height2 = IslandHeightFunction(triangle.p2.x, triangle.p2.y);
        float height2dx = IslandHeightFunction(triangle.p2.x + epsilon, triangle.p2.y);
        float height2dy = IslandHeightFunction(triangle.p2.x, triangle.p2.y + epsilon);

        glm::vec3 vertex2 = glm::vec3(triangle.p2.x, height2, triangle.p2.y);
        glm::vec3 normal2 = GetNormalFromHeightSamples(height2, height2dx, height2dy, epsilon);
        glm::vec2 uv2 = glm::vec2(triangle.p2.x, triangle.p2.y) * 0.25f;

        float height3 = IslandHeightFunction(triangle.p3.x, triangle.p3.y);
        float height3dx = IslandHeightFunction(triangle.p3.x + epsilon, triangle.p3.y);
        float height3dy = IslandHeightFunction(triangle.p3.x, triangle.p3.y + epsilon);
        
        glm::vec3 vertex3 = glm::vec3(triangle.p3.x, height3, triangle.p3.y);
        glm::vec3 normal3 = GetNormalFromHeightSamples(height3, height3dx, height3dy, epsilon);
        glm::vec2 uv3 = glm::vec2(triangle.p3.x, triangle.p3.y) * 0.25f;

        positionAttribute->AddData(vertex1);
        positionAttribute->AddData(vertex2);
        positionAttribute->AddData(vertex3);

        normalAttribute->AddData(normal1);
        normalAttribute->AddData(normal2);
        normalAttribute->AddData(normal3);

        uvAttribute->AddData(uv1);
        uvAttribute->AddData(uv2);
        uvAttribute->AddData(uv3);

        unsigned int index = static_cast<unsigned int>(indices.size());

        indices.push_back(index);
        indices.push_back(index + 2);
        indices.push_back(index + 1);
    }

    m_IslandMesh.SetAttribute(0, positionAttribute);
    m_IslandMesh.SetAttribute(1, normalAttribute);
    m_IslandMesh.SetAttribute(2, uvAttribute);
    m_IslandMesh.SetIndecies(indices);

    m_IslandMesh.GenerateDeviceBuffers();

    MeshRenderComponent *meshRenderer = AddComponent<MeshRenderComponent>();

    meshRenderer->mesh = &m_IslandMesh;
    meshRenderer->material = m_Material;
}
