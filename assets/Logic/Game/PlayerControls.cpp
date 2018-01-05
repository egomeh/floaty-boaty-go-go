#include "PlayerControls.hpp"

#include "ShipMovement.hpp"
#include "FreeFlight.hpp"
#include "Torpedo.hpp"

#include "input.hpp"

#include "glm/gtx/vector_angle.hpp"

void PlayerControls::Update()
{
    if (Input::KeyPressed('u'))
    {
        m_ControlsEnabled = !m_ControlsEnabled;
    }

    FreeFlight *freeFlight = m_EntityDatabase->GetComponent<FreeFlight>(m_CameraHandle);

    if (freeFlight)
    {
        freeFlight->SetFreeeMovement(!m_ControlsEnabled);
    }

    if (!m_ControlsEnabled)
    {
        return;
    }

    ShipMovement *shipMovement = GetComponent<ShipMovement>();
    Transform *transform = GetComponent<Transform>();

    if (shipMovement)
    {
        shipMovement->SetThrottle(Input::KeyValue('w'));
        shipMovement->SetTurn(Input::KeyValue('a') - Input::KeyValue('d'));
    }

    if (Input::KeyPressed('f'))
    {
        int height = shipMovement->GetTargetHeight();
        height += 1;

        shipMovement->SetTargetHeight(static_cast<HeightLayer>(height));
    }

    if (Input::KeyPressed('v'))
    {
        int height = shipMovement->GetTargetHeight();
        height -= 1;

        shipMovement->SetTargetHeight(static_cast<HeightLayer>(height));
    }

    auto frameContext = GetFrameContext();

    m_TorpedoCooldownTimer = std::max(-1.f, m_TorpedoCooldownTimer - frameContext.deltaTime);

    if (Input::KeyPressed(' ') && m_TorpedoCooldownTimer < 0.f)
    {
        FireTorpedo();
        m_TorpedoCooldownTimer = m_TorpedoCooldown;
    }
}

void PlayerControls::LateUpdate()
{
    if (!m_ControlsEnabled)
    {
        return;
    }

    Transform *transform = GetComponent<Transform>();
    Transform *cameraTransform = m_EntityDatabase->GetComponent<Transform>(m_CameraHandle);

    glm::vec3 cameraForward = cameraTransform->GetLocalForwards();
    glm::vec3 playerPosition = transform->GetLocalPosition();

    glm::vec3 cameraPosition = playerPosition - cameraForward * 8.0f;
    cameraPosition += glm::vec3(0.0f, 1.0f, 0.0f);

    cameraTransform->SetLocalPosition(cameraPosition);
}

void PlayerControls::Start()
{
    m_CameraHandle = m_EntityDatabase->FindEntitiesByName("camera")[0];

    FreeFlight *cameraFreeFlight = m_EntityDatabase->GetComponent<FreeFlight>(m_CameraHandle);

    if (cameraFreeFlight != nullptr)
    {
        cameraFreeFlight->SetFreeeMovement(false);
    }

    m_TorpedoPrefab = m_AssetDatabase->RequestAsset<Prefab>(m_TorpedoPrefabName);

    m_TorpedoPrefabName.clear();

    m_TorpedoCooldownTimer = 0.f;
}

void PlayerControls::OnDestroy()
{
    FreeFlight *cameraFreeFlight = m_EntityDatabase->GetComponent<FreeFlight>(m_CameraHandle);

    if (cameraFreeFlight != nullptr)
    {
        cameraFreeFlight->SetFreeeMovement(true);
    }
}

inline void PlayerControls::FireTorpedo()
{
    EntityHandle torpedoEntity = Instantiate(m_TorpedoPrefab);

    Transform *torpedoTransform = m_EntityDatabase->GetComponent<Transform>(torpedoEntity);
    Transform *cameraTransform = m_EntityDatabase->GetComponent<Transform>(m_CameraHandle);
    Transform *transform = GetComponent<Transform>();

    Torpedo *torpedo = m_EntityDatabase->GetComponent<Torpedo>(torpedoEntity);

    torpedoTransform->SetLocalPosition(transform->GetLocalPosition());
    torpedoTransform->SetLocalRotation(transform->GetLocalRotation());

    glm::vec3 shipForwards = transform->GetLocalForwards();
    glm::vec3 cameraForwards = cameraTransform->GetLocalForwards();

    glm::vec2 shipProjection = glm::normalize(glm::vec2(shipForwards.x, shipForwards.z));
    glm::vec2 cameraProjection = glm::normalize(glm::vec2(cameraForwards.x, cameraForwards.z));

    float torpedoRotateAngle = glm::orientedAngle(shipProjection, cameraProjection);

    torpedo->SetRotation(-torpedoRotateAngle);
}
