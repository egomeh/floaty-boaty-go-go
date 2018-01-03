#pragma once

#include "logic.hpp"
#include "util.hpp"

class ChangeScene : public LogicComponent
{
public:
    ChangeScene()
    {
        m_Timer = 0.f;
        m_IsDeleting = false;
    }

    void Update() override;
    void Start() override;

    template<typename SerializerType>
    void Deserialize(SerializerType serializer)
    {
        DESERIALIZE(m_TargetScene, serializer);
        DESERIALIZE(m_Time, serializer);
    }

private:
    std::string m_TargetScene;

    float m_Timer;
    float m_Time;
    bool m_IsDeleting;
};
