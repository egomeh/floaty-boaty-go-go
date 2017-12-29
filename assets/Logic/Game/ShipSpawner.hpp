#pragma once

#include "logic.hpp"

#include "Graphics/mesh.hpp"

class ShipSpawner : public LogicComponent
{
public:
    ShipSpawner();

    void Update() override;
    void Start() override;

    template<typename SerializerType>
    void Deserialize(SerializerType serializer)
    {
        DESERIALIZE(m_AirshipPrefabName, serializer);
        DESERIALIZE(m_SpawnTime, serializer);
        DESERIALIZE(m_SpawnRadius, serializer);
    }

private:
    std::string m_AirshipPrefabName;

    float m_SpawnTime;
    float m_SpawnTimer;
    float m_SpawnRadius;

    Prefab *m_AirshipPrefab;
};


