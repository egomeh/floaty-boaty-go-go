#include "TurbulenceWobble.hpp"

// STL includes
#include <random>

// Engine includes
#include "transform.hpp"

// GLM includes
#include "glm/glm.hpp"
#include "glm/gtc/noise.hpp"
#include "glm/gtc/random.hpp"

TurbulenceWobble::TurbulenceWobble()
{
    m_WobbleAngle = glm::vec3(0.f, 0.f, 0.f);
    m_WobbleDisplacement = glm::vec3(0.f, 0.f, 0.f);
}

void TurbulenceWobble::Start()
{
    std::default_random_engine generator;
    std::uniform_real_distribution<float> distribution(0.f, 1.f);
    m_RandomNumberFromStart = distribution(generator);
}

void TurbulenceWobble::Update()
{
    Transform *transform = GetComponent<Transform>();

    const float &deltaTime = GetFrameContext().deltaTime;

    m_WobbleProgress += deltaTime / m_WobbleTime;

    float sinWobble = std::sin(m_WobbleProgress);
    float sinWobble2 = std::sin(m_WobbleProgress * 2.f);
    float cosWobble = std::cos(m_WobbleProgress);
    float cosWobble2 = std::cos(m_WobbleProgress * 2.f);

    glm::vec3 position = transform->GetLocalPosition();

    float rotation1 = 0.1f * glm::perlin(glm::vec2(sinWobble + position.x * 0.125f, sinWobble2 + position.z * 0.026f));
    float rotation2 = 0.1f * glm::perlin(glm::vec2(cosWobble + position.y * 0.109f, sinWobble2 + position.x * 0.04f));

    glm::vec3 positionOffset;
    positionOffset.x = 0.1f * glm::perlin(glm::vec2(sinWobble + position.x * 0.125f, sinWobble2 + position.z * 0.026f));
    positionOffset.y = 0.1f * glm::perlin(glm::vec2(sinWobble2 + position.y * 0.209f, cosWobble2 + position.x * 0.4f));
    positionOffset.z = 0.1f * glm::perlin(glm::vec2(cosWobble2 + position.y * 0.189f, cosWobble2 + position.x * 0.14f));

    glm::quat rotation = glm::quat(glm::vec3(rotation1, 0.0f, rotation2));

    transform->SetLocalRotation(rotation);
    transform->SetLocalPosition(positionOffset);
}
