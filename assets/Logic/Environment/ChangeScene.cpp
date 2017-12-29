
#include "ChangeScene.hpp"

#include "input.hpp"

void ChangeScene::Update()
{
    if (Input::KeyPressed(' '))
    {
        SceneData *targetScene = m_AssetDatabase->RequestAsset<SceneData>(m_TargetScene);

        LoadSceneContent(*targetScene, *m_EntityDatabase, *m_ComponentLoaderSystem);

        m_EntityDatabase->MarkEntityForDestruction(owner);
    }
}

void ChangeScene::Start()
{
}
