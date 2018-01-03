#include "FXAA.hpp"

#include "application.hpp"

void FXAA::Update()
{
}

void FXAA::Start()
{
    const std::string materialName = "FXAA";
    m_Material = m_AssetDatabase->RequestAsset<Material>(materialName);

    if (!m_Material)
    {
        return;
    }

    Camera *camera = GetComponent<Camera>();

    if (!camera)
    {
        return;
    }

    PostProcess self;
    self.logicComponent = this;
    self.order = 1;

    camera->postProcessHooks.push_back(self);
    std::sort(camera->postProcessHooks.begin(), camera->postProcessHooks.end());
}

void FXAA::OnRenderImage(Texture *source, RenderTexture *destination)
{
    Application *application = Application::Instance();
    application->GetRenderer()->Blit(source, destination, *m_Material);
}
