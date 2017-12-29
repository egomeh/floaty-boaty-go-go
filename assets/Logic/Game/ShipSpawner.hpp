#pragma once

#include "logic.hpp"

#include "Graphics/mesh.hpp"

class ShipSpawner : public LogicComponent
{
public:
    ShipSpawner();

    void Update() override;
    void Start() override;

    void NotifyShipDestroyed(const EntityHandle &handle);

    template<typename SerializerType>
    void Deserialize(SerializerType serializer)
    {
        DESERIALIZE(m_AirshipPrefabName, serializer);
        DESERIALIZE(m_SpawnTime, serializer);
        DESERIALIZE(m_SpawnRadius, serializer);
        DESERIALIZE(m_MaxAirships, serializer);
    }

private:
    std::string m_AirshipPrefabName;

    int m_MaxAirships;

    float m_SpawnTime;
    float m_SpawnTimer;
    float m_SpawnRadius;

    Prefab *m_AirshipPrefab;

    std::vector<EntityHandle> m_SpawnedShips;
};


