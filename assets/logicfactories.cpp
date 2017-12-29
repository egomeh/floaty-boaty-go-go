#include "logicfactories.hpp"
#include "serialization/jsonserializer.hpp"

MeshRandomizer *MeshRandomizerLogicFactory::CreatePointer()
{
    return new MeshRandomizer();
}

MeshRandomizer *MeshRandomizerLogicFactory::CreatePointer(const nlohmann::json &serializedComponent)
{
    MeshRandomizer* component = new MeshRandomizer();

    JsonSerializer serializer(serializedComponent);

    component->Deserialize(serializer);

    return component;
}

OverlayImage *OverlayImageLogicFactory::CreatePointer()
{
    return new OverlayImage();
}

OverlayImage *OverlayImageLogicFactory::CreatePointer(const nlohmann::json &serializedComponent)
{
    OverlayImage* component = new OverlayImage();

    JsonSerializer serializer(serializedComponent);

    component->Deserialize(serializer);

    return component;
}

Torpedo *TorpedoLogicFactory::CreatePointer()
{
    return new Torpedo();
}

Torpedo *TorpedoLogicFactory::CreatePointer(const nlohmann::json &serializedComponent)
{
    Torpedo* component = new Torpedo();

    JsonSerializer serializer(serializedComponent);

    component->Deserialize(serializer);

    return component;
}

Island *IslandLogicFactory::CreatePointer()
{
    return new Island();
}

Island *IslandLogicFactory::CreatePointer(const nlohmann::json &serializedComponent)
{
    Island* component = new Island();

    JsonSerializer serializer(serializedComponent);

    component->Deserialize(serializer);

    return component;
}

ChangeScene *ChangeSceneLogicFactory::CreatePointer()
{
    return new ChangeScene();
}

ChangeScene *ChangeSceneLogicFactory::CreatePointer(const nlohmann::json &serializedComponent)
{
    ChangeScene* component = new ChangeScene();

    JsonSerializer serializer(serializedComponent);

    component->Deserialize(serializer);

    return component;
}

PropellerRotator *PropellerRotatorLogicFactory::CreatePointer()
{
    return new PropellerRotator();
}

PropellerRotator *PropellerRotatorLogicFactory::CreatePointer(const nlohmann::json &serializedComponent)
{
    PropellerRotator* component = new PropellerRotator();

    JsonSerializer serializer(serializedComponent);

    component->Deserialize(serializer);

    return component;
}

FreeFlight *FreeFlightLogicFactory::CreatePointer()
{
    return new FreeFlight();
}

FreeFlight *FreeFlightLogicFactory::CreatePointer(const nlohmann::json &serializedComponent)
{
    FreeFlight* component = new FreeFlight();

    JsonSerializer serializer(serializedComponent);

    component->Deserialize(serializer);

    return component;
}

Water *WaterLogicFactory::CreatePointer()
{
    return new Water();
}

Water *WaterLogicFactory::CreatePointer(const nlohmann::json &serializedComponent)
{
    Water* component = new Water();

    JsonSerializer serializer(serializedComponent);

    component->Deserialize(serializer);

    return component;
}

PlayerControls *PlayerControlsLogicFactory::CreatePointer()
{
    return new PlayerControls();
}

PlayerControls *PlayerControlsLogicFactory::CreatePointer(const nlohmann::json &serializedComponent)
{
    PlayerControls* component = new PlayerControls();

    JsonSerializer serializer(serializedComponent);

    component->Deserialize(serializer);

    return component;
}

ShipMovement *ShipMovementLogicFactory::CreatePointer()
{
    return new ShipMovement();
}

ShipMovement *ShipMovementLogicFactory::CreatePointer(const nlohmann::json &serializedComponent)
{
    ShipMovement* component = new ShipMovement();

    JsonSerializer serializer(serializedComponent);

    component->Deserialize(serializer);

    return component;
}

Rotator *RotatorLogicFactory::CreatePointer()
{
    return new Rotator();
}

Rotator *RotatorLogicFactory::CreatePointer(const nlohmann::json &serializedComponent)
{
    Rotator* component = new Rotator();

    JsonSerializer serializer(serializedComponent);

    component->Deserialize(serializer);

    return component;
}

TurbulenceWobble *TurbulenceWobbleLogicFactory::CreatePointer()
{
    return new TurbulenceWobble();
}

TurbulenceWobble *TurbulenceWobbleLogicFactory::CreatePointer(const nlohmann::json &serializedComponent)
{
    TurbulenceWobble* component = new TurbulenceWobble();

    JsonSerializer serializer(serializedComponent);

    component->Deserialize(serializer);

    return component;
}

AIControls *AIControlsLogicFactory::CreatePointer()
{
    return new AIControls();
}

AIControls *AIControlsLogicFactory::CreatePointer(const nlohmann::json &serializedComponent)
{
    AIControls* component = new AIControls();

    JsonSerializer serializer(serializedComponent);

    component->Deserialize(serializer);

    return component;
}

ShipSpawner *ShipSpawnerLogicFactory::CreatePointer()
{
    return new ShipSpawner();
}

ShipSpawner *ShipSpawnerLogicFactory::CreatePointer(const nlohmann::json &serializedComponent)
{
    ShipSpawner* component = new ShipSpawner();

    JsonSerializer serializer(serializedComponent);

    component->Deserialize(serializer);

    return component;
}

LogicComponentFactories g_LogicComponentFactories;

void RegisterLogicFactories(LogicSystem &logicSystem)
{
    logicSystem.RegisterFactory<MeshRandomizer>("MeshRandomizer", &g_LogicComponentFactories.m_MeshRandomizerLogicFactory);
    logicSystem.RegisterFactory<OverlayImage>("OverlayImage", &g_LogicComponentFactories.m_OverlayImageLogicFactory);
    logicSystem.RegisterFactory<Torpedo>("Torpedo", &g_LogicComponentFactories.m_TorpedoLogicFactory);
    logicSystem.RegisterFactory<Island>("Island", &g_LogicComponentFactories.m_IslandLogicFactory);
    logicSystem.RegisterFactory<ChangeScene>("ChangeScene", &g_LogicComponentFactories.m_ChangeSceneLogicFactory);
    logicSystem.RegisterFactory<PropellerRotator>("PropellerRotator", &g_LogicComponentFactories.m_PropellerRotatorLogicFactory);
    logicSystem.RegisterFactory<FreeFlight>("FreeFlight", &g_LogicComponentFactories.m_FreeFlightLogicFactory);
    logicSystem.RegisterFactory<Water>("Water", &g_LogicComponentFactories.m_WaterLogicFactory);
    logicSystem.RegisterFactory<PlayerControls>("PlayerControls", &g_LogicComponentFactories.m_PlayerControlsLogicFactory);
    logicSystem.RegisterFactory<ShipMovement>("ShipMovement", &g_LogicComponentFactories.m_ShipMovementLogicFactory);
    logicSystem.RegisterFactory<Rotator>("Rotator", &g_LogicComponentFactories.m_RotatorLogicFactory);
    logicSystem.RegisterFactory<TurbulenceWobble>("TurbulenceWobble", &g_LogicComponentFactories.m_TurbulenceWobbleLogicFactory);
    logicSystem.RegisterFactory<AIControls>("AIControls", &g_LogicComponentFactories.m_AIControlsLogicFactory);
    logicSystem.RegisterFactory<ShipSpawner>("ShipSpawner", &g_LogicComponentFactories.m_ShipSpawnerLogicFactory);
}
