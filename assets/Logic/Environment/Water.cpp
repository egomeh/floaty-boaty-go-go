#include "water.hpp"

#include "util.hpp"
#include "transform.hpp"
#include "Graphics/meshrenderer.hpp"

Water::Water()
{

}

void Water::Update()
{
}

void Water::Start()
{
    auto positionAttribute = std::make_shared<BasicMeshAttribute<glm::vec3>>(Float, 3);
    positionAttribute->AddData(glm::vec3(-1, 0, -1));
    positionAttribute->AddData(glm::vec3(-1, 0, +1));
    positionAttribute->AddData(glm::vec3(+1, 0, +1));
    positionAttribute->AddData(glm::vec3(+1, 0, -1));

    auto normalAttribute = std::make_shared<BasicMeshAttribute<glm::vec3>>(Float, 3);
    normalAttribute->AddData(glm::vec3(0, 1, 0));
    normalAttribute->AddData(glm::vec3(0, 1, 0));
    normalAttribute->AddData(glm::vec3(0, 1, 0));
    normalAttribute->AddData(glm::vec3(0, 1, 0));

    auto uvAttribute = std::make_shared<BasicMeshAttribute<glm::vec2>>(Float, 2);
    uvAttribute->AddData(glm::vec2(0, 0));
    uvAttribute->AddData(glm::vec2(0, 1));
    uvAttribute->AddData(glm::vec2(1, 1));
    uvAttribute->AddData(glm::vec2(1, 0));

    std::vector<unsigned int> indices{0, 2, 1, 0, 3, 2};

    m_WaterPlane.SetAttribute(0, positionAttribute);
    m_WaterPlane.SetAttribute(1, normalAttribute);
    m_WaterPlane.SetAttribute(2, uvAttribute);
    m_WaterPlane.SetIndecies(indices);

    m_WaterPlane.GenerateDeviceBuffers();

    MeshRenderComponent *meshRenderer = AddComponent<MeshRenderComponent>();
    
    Material *material = m_AssetDatabase->RequestAsset<Material>(m_MaterialName);

    meshRenderer->mesh = &m_WaterPlane;
    meshRenderer->material = material;

    m_MaterialName.clear();
}
