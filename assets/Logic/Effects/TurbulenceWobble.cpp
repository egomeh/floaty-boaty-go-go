#include "TurbulenceWobble.hpp"

#include "transform.hpp"

#include "glm/gtc/random.hpp"
#include "glm/glm.hpp"

TurbulenceWobble::TurbulenceWobble()
{
    m_WobbleAngle = glm::vec3(0.f, 0.f, 0.f);
    m_WobbleDisplacement = glm::vec3(0.f, 0.f, 0.f);
}

void TurbulenceWobble::Update()
{
    Transform *transform = GetComponent<Transform>();

    const float &deltaTime = GetFrameContext().deltaTime;

    m_WobbleProgress += deltaTime / m_WobbleTime;

    if (m_WobbleProgress >= 1.0f)
    {
        transform->SetLocalEulerAngles(m_WobbleAngle);
        transform->SetLocalPosition(m_WobbleDisplacement);

        PickRandomDisplacement();
    }
    else
    {
        float lerpFactor = m_WobbleProgress;

        glm::vec3 lerpAngles = lerp(m_OldWobbleAngle, m_WobbleAngle, lerpFactor);
        glm::vec3 lerpPosition = lerp(m_OldDisplacement, m_WobbleDisplacement, lerpFactor);

        transform->SetLocalEulerAngles(lerpAngles);
        transform->SetLocalPosition(lerpPosition);
    }
}

void TurbulenceWobble::PickRandomDisplacement()
{
    m_WobbleProgress = 0.0f;

    m_OldDisplacement = m_WobbleDisplacement;
    m_OldWobbleAngle = m_WobbleAngle;

    m_WobbleAngle = glm::gaussRand(glm::vec3(0.0f), glm::vec3(1.0f));
    m_WobbleDisplacement = glm::gaussRand(glm::vec3(0.0f), glm::vec3(.3f));
}
