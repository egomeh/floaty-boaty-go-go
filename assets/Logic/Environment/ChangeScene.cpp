#include "ChangeScene.hpp"

#include "input.hpp"
#include "../Effects/OverlayImage.hpp"

void ChangeScene::Update()
{
    if (Input::KeyPressed(' ') && !m_IsDeleting)
    {
        SceneData *targetScene = m_AssetDatabase->RequestAsset<SceneData>(m_TargetScene);

        LoadSceneContent(*targetScene, *m_EntityDatabase, *m_ComponentLoaderSystem);

        m_IsDeleting = true;
    }

    if (m_IsDeleting)
    {
        m_Timer += GetFrameContext().deltaTime / m_Time;

        OverlayImage *overlayImage = GetComponent<OverlayImage>();

        if (overlayImage)
        {
            overlayImage->SetTransparency(1.0f - m_Timer);
        }

        if (m_Timer > 1.0f)
        {
            m_EntityDatabase->MarkEntityForDestruction(owner);
        }
    }
}

void ChangeScene::Start()
{
}
