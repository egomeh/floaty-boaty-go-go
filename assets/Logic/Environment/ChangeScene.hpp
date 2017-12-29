#pragma once

#include "logic.hpp"
#include "util.hpp"

class ChangeScene : public LogicComponent
{
public:
    ChangeScene()
    {
    }

    void Update() override;
    void Start() override;

    template<typename SerializerType>
    void Deserialize(SerializerType serializer)
    {
        DESERIALIZE(m_TargetScene, serializer);
    }

private:
    std::string m_TargetScene;
};
