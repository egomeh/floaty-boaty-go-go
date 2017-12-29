#pragma once

#include "logic.hpp"
#include "scene.hpp"

#include <windows.h>
#include "util.hpp"
#include <sstream>

class PlayerControls : public LogicComponent
{
public:
    PlayerControls()
    {
    }

    void Update() override;
    void LateUpdate() override;
    void Start() override;
    void OnDestroy() override;

    template<typename SerializerType>
    void Deserialize(SerializerType serializer)
    {
        DESERIALIZE(m_TorpedoPrefabName, serializer);
        DESERIALIZE(m_TorpedoCooldown, serializer);
    }

private:
    inline void FireTorpedo();

    EntityHandle m_CameraHandle;

    std::string m_TorpedoPrefabName;

    Prefab *m_TorpedoPrefab;

    bool m_ControlsEnabled;

    float m_TorpedoCooldown;
    float m_TorpedoCooldownTimer;
};
