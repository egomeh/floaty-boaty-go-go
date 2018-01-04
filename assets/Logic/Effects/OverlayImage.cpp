#include "OverlayImage.hpp"

#include "Graphics/texture.hpp"
#include "Graphics/meshrenderer.hpp"

void OverlayImage::Update()
{
}

void OverlayImage::Start()
{
    Texture *overlayTexture = m_AssetDatabase->RequestAsset<Texture>(m_ImageName);
    Shader *shader = m_AssetDatabase->RequestAsset<Shader>("overlay");
    m_Material = std::make_shared<Material>(shader);

    m_Material->GetUniformContext().SetTexture(std::hash<std::string>()("_Image"), overlayTexture);

    std::shared_ptr<BasicMeshAttribute<glm::vec3>> positionAttribute = std::make_shared<BasicMeshAttribute<glm::vec3>>(Float, 3);
    positionAttribute->AddData(glm::vec3(-1, -1, 1));
    positionAttribute->AddData(glm::vec3(3, -1, 1));
    positionAttribute->AddData(glm::vec3(-1, 3, 1));

    std::shared_ptr<BasicMeshAttribute<glm::vec2>> uvAttribute = std::make_shared<BasicMeshAttribute<glm::vec2>>(Float, 2);
    uvAttribute->AddData(glm::vec2(0, 0));
    uvAttribute->AddData(glm::vec2(2, 0));
    uvAttribute->AddData(glm::vec2(0, 2));

    std::vector<unsigned int> ind{0, 2, 1};

    m_Mesh.SetAttribute(0, positionAttribute);
    m_Mesh.SetAttribute(1, uvAttribute);
    m_Mesh.SetIndecies(ind);

    m_Mesh.GenerateDeviceBuffers();

    MeshRenderComponent *meshRenderer = AddComponent<MeshRenderComponent>();

    meshRenderer->material = m_Material.get();
    meshRenderer->mesh = &m_Mesh;
    meshRenderer->renderQueue = RenderQueue::Transparent;

    meshRenderer->material->GetUniformContext().SetFloat(std::hash<std::string>()("_Transparency"), 1.0f);
}

void OverlayImage::SetTransparency(float transparency)
{
    MeshRenderComponent *meshRenderer = GetComponent<MeshRenderComponent>();

    if (!meshRenderer)
    {
        return;
    }

    meshRenderer->material->GetUniformContext().SetFloat(std::hash<std::string>()("_Transparency"), transparency);
}
