#include "MeshRandomizer.hpp"

#include "util.hpp"
#include "transform.hpp"
#include "Graphics/meshrenderer.hpp"

#include <random>

MeshRandomizer::MeshRandomizer()
{
}

void MeshRandomizer::Update()
{
}

void MeshRandomizer::Start()
{
    std::mt19937 rng;
    rng.seed(std::random_device()());
    std::uniform_int_distribution<std::mt19937::result_type> distribution(0, 2);

    int diceRoll = distribution(rng);

    if (m_MesnNames[diceRoll].compare("") != 0)
    {
        Mesh *mesh = m_AssetDatabase->RequestAsset<Mesh>(m_MesnNames[diceRoll]);
        Material *material = m_AssetDatabase->RequestAsset<Material>(m_MaterialName);

        MeshRenderComponent *meshRenderer = AddComponent<MeshRenderComponent>();

        meshRenderer->mesh = mesh;
        meshRenderer->material = material;
    }
}
