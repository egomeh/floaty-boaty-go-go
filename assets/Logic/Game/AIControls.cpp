#include "AIControls.hpp"

#include "ShipMovement.hpp"
#include "FreeFlight.hpp"
#include "Torpedo.hpp"

#include "input.hpp"

#include "glm/gtx/vector_angle.hpp"

void AIControls::Update()
{
    Transform *transform = GetComponent<Transform>();

    glm::vec3 position = transform->GetLocalPosition();

    position.y = 0.0f;

    // stop the ship when reaching the center
    if (glm::length(position) < 1.0f)
    {
        ShipMovement *shipMovement = GetComponent<ShipMovement>();

        shipMovement->SetThrottle(0.0f);
    }
}

void AIControls::Start()
{
    ShipMovement *shipMovement = GetComponent<ShipMovement>();
    shipMovement->SetThrottle(1.0f);
}
