#include "PropellerRotator.hpp"

#include "util.hpp"
#include "transform.hpp"

PropellerRotator::PropellerRotator()
{
}

void PropellerRotator::Update()
{
    Transform *transform = GetComponent<Transform>();

    glm::quat rotation = transform->GetLocalRotation();

    rotation *= glm::quat(m_EulerRotation * GetFrameContext().deltaTime);

    transform->SetLocalRotation(rotation);
}
