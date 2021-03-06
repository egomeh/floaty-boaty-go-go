#include "TextDisplay.hpp"

#include "Graphics/meshrenderer.hpp"

void TextDisplay::Update()
{
}

void TextDisplay::Start()
{
    DebugLog(m_DisplayText);

    FontTexture *fontTexture = m_AssetDatabase->RequestAsset<FontTexture>("default");
    m_Material.SetShader(m_AssetDatabase->RequestAsset<Shader>("text"));
    m_Material.SetBlendMode(ShaderBlendMode::Alpha);

    m_Material.GetUniformContext().SetTexture(std::hash<std::string>()("_TextureAtlas"), fontTexture->GetTexture());

    fontTexture->GenerateTextMesh(m_Mesh, m_DisplayText, m_TextAlign);
    glm::vec3 textMeshBounds = m_Mesh.GetBounds();

    MeshRenderComponent *meshRenderer = AddComponent<MeshRenderComponent>();

    meshRenderer->mesh = &m_Mesh;
    meshRenderer->material = &m_Material;
    meshRenderer->renderQueue = RenderQueue::Overlay;
}
