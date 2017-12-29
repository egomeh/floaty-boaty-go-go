#include "ShipSpawner.hpp"

#include "util.hpp"
#include "transform.hpp"
#include "Graphics/meshrenderer.hpp"

#include "glm/gtx/quaternion.hpp"

#include "input.hpp"

#include <random>

ShipSpawner::ShipSpawner()
{
}

void ShipSpawner::Update()
{
    m_SpawnTimer -= GetFrameContext().deltaTime / m_SpawnTime;

    if (Input::KeyPressed('t')) // if (m_SpawnTimer < 0.0f)
    {
        EntityHandle spawnedShip = Instantiate(m_AirshipPrefab);

        Transform *transform = m_EntityDatabase->GetComponent<Transform>(spawnedShip);

        std::mt19937 rng;
        rng.seed(std::random_device()());
        std::uniform_real_distribution<float> distribution(0.0f, 1.1f);

        float angle = 3.14159f * 2.0f * distribution(rng);

        glm::vec3 spawnPosition = glm::vec3(std::cos(angle), 0.0f, std::sin(angle)) * m_SpawnRadius;
        glm::vec3 lookDirection = glm::normalize(spawnPosition);

        spawnPosition.y = 10.0f;

        glm::quat rotation = glm::quatLookAt(lookDirection, glm::vec3(0.0f, 1.0f, 0.0f));

        transform->SetLocalPosition(spawnPosition);
        transform->SetLocalRotation(rotation);

        m_SpawnTimer = m_SpawnTime;
    }
}

void ShipSpawner::Start()
{
    m_AirshipPrefab = m_AssetDatabase->RequestAsset<Prefab>(m_AirshipPrefabName);
    m_SpawnTimer = 0;
}
