#pragma once

#include "logic.hpp"
#include "assetdatabase.hpp"

#include "Logic/Effects/MeshRandomizer.hpp"
#include "Logic/Effects/OverlayImage.hpp"
#include "Logic/Game/Torpedo.hpp"
#include "Logic/Environment/Island.hpp"
#include "Logic/Environment/ChangeScene.hpp"
#include "Logic/Effects/PropellerRotator.hpp"
#include "Logic/Game/FreeFlight.hpp"
#include "Logic/Environment/Water.hpp"
#include "Logic/Effects/PostProcess/FXAA.hpp"
#include "Logic/Game/PlayerControls.hpp"
#include "Logic/Game/ShipMovement.hpp"
#include "Logic/Effects/Rotator.hpp"
#include "Logic/Effects/TurbulenceWobble.hpp"
#include "Logic/Game/AIControls.hpp"
#include "Logic/Game/WeaponsController.hpp"
#include "Logic/Game/ShipSpawner.hpp"

void RegisterLogicFactories(LogicSystem &logicSystem);

class MeshRandomizerLogicFactory : public LogicComponentFactory<MeshRandomizer>
{
public:
    MeshRandomizerLogicFactory() : LogicComponentFactory<MeshRandomizer>()
    {
    }

    MeshRandomizer *CreatePointer() override;
    MeshRandomizer *CreatePointer(const nlohmann::json &serializedComponent) override;
};

class OverlayImageLogicFactory : public LogicComponentFactory<OverlayImage>
{
public:
    OverlayImageLogicFactory() : LogicComponentFactory<OverlayImage>()
    {
    }

    OverlayImage *CreatePointer() override;
    OverlayImage *CreatePointer(const nlohmann::json &serializedComponent) override;
};

class TorpedoLogicFactory : public LogicComponentFactory<Torpedo>
{
public:
    TorpedoLogicFactory() : LogicComponentFactory<Torpedo>()
    {
    }

    Torpedo *CreatePointer() override;
    Torpedo *CreatePointer(const nlohmann::json &serializedComponent) override;
};

class IslandLogicFactory : public LogicComponentFactory<Island>
{
public:
    IslandLogicFactory() : LogicComponentFactory<Island>()
    {
    }

    Island *CreatePointer() override;
    Island *CreatePointer(const nlohmann::json &serializedComponent) override;
};

class ChangeSceneLogicFactory : public LogicComponentFactory<ChangeScene>
{
public:
    ChangeSceneLogicFactory() : LogicComponentFactory<ChangeScene>()
    {
    }

    ChangeScene *CreatePointer() override;
    ChangeScene *CreatePointer(const nlohmann::json &serializedComponent) override;
};

class PropellerRotatorLogicFactory : public LogicComponentFactory<PropellerRotator>
{
public:
    PropellerRotatorLogicFactory() : LogicComponentFactory<PropellerRotator>()
    {
    }

    PropellerRotator *CreatePointer() override;
    PropellerRotator *CreatePointer(const nlohmann::json &serializedComponent) override;
};

class FreeFlightLogicFactory : public LogicComponentFactory<FreeFlight>
{
public:
    FreeFlightLogicFactory() : LogicComponentFactory<FreeFlight>()
    {
    }

    FreeFlight *CreatePointer() override;
    FreeFlight *CreatePointer(const nlohmann::json &serializedComponent) override;
};

class WaterLogicFactory : public LogicComponentFactory<Water>
{
public:
    WaterLogicFactory() : LogicComponentFactory<Water>()
    {
    }

    Water *CreatePointer() override;
    Water *CreatePointer(const nlohmann::json &serializedComponent) override;
};

class FXAALogicFactory : public LogicComponentFactory<FXAA>
{
public:
    FXAALogicFactory() : LogicComponentFactory<FXAA>()
    {
    }

    FXAA *CreatePointer() override;
    FXAA *CreatePointer(const nlohmann::json &serializedComponent) override;
};

class PlayerControlsLogicFactory : public LogicComponentFactory<PlayerControls>
{
public:
    PlayerControlsLogicFactory() : LogicComponentFactory<PlayerControls>()
    {
    }

    PlayerControls *CreatePointer() override;
    PlayerControls *CreatePointer(const nlohmann::json &serializedComponent) override;
};

class ShipMovementLogicFactory : public LogicComponentFactory<ShipMovement>
{
public:
    ShipMovementLogicFactory() : LogicComponentFactory<ShipMovement>()
    {
    }

    ShipMovement *CreatePointer() override;
    ShipMovement *CreatePointer(const nlohmann::json &serializedComponent) override;
};

class RotatorLogicFactory : public LogicComponentFactory<Rotator>
{
public:
    RotatorLogicFactory() : LogicComponentFactory<Rotator>()
    {
    }

    Rotator *CreatePointer() override;
    Rotator *CreatePointer(const nlohmann::json &serializedComponent) override;
};

class TurbulenceWobbleLogicFactory : public LogicComponentFactory<TurbulenceWobble>
{
public:
    TurbulenceWobbleLogicFactory() : LogicComponentFactory<TurbulenceWobble>()
    {
    }

    TurbulenceWobble *CreatePointer() override;
    TurbulenceWobble *CreatePointer(const nlohmann::json &serializedComponent) override;
};

class AIControlsLogicFactory : public LogicComponentFactory<AIControls>
{
public:
    AIControlsLogicFactory() : LogicComponentFactory<AIControls>()
    {
    }

    AIControls *CreatePointer() override;
    AIControls *CreatePointer(const nlohmann::json &serializedComponent) override;
};

class WeaponsControllerLogicFactory : public LogicComponentFactory<WeaponsController>
{
public:
    WeaponsControllerLogicFactory() : LogicComponentFactory<WeaponsController>()
    {
    }

    WeaponsController *CreatePointer() override;
    WeaponsController *CreatePointer(const nlohmann::json &serializedComponent) override;
};

class ShipSpawnerLogicFactory : public LogicComponentFactory<ShipSpawner>
{
public:
    ShipSpawnerLogicFactory() : LogicComponentFactory<ShipSpawner>()
    {
    }

    ShipSpawner *CreatePointer() override;
    ShipSpawner *CreatePointer(const nlohmann::json &serializedComponent) override;
};

struct LogicComponentFactories
{
    MeshRandomizerLogicFactory m_MeshRandomizerLogicFactory;
    OverlayImageLogicFactory m_OverlayImageLogicFactory;
    TorpedoLogicFactory m_TorpedoLogicFactory;
    IslandLogicFactory m_IslandLogicFactory;
    ChangeSceneLogicFactory m_ChangeSceneLogicFactory;
    PropellerRotatorLogicFactory m_PropellerRotatorLogicFactory;
    FreeFlightLogicFactory m_FreeFlightLogicFactory;
    WaterLogicFactory m_WaterLogicFactory;
    FXAALogicFactory m_FXAALogicFactory;
    PlayerControlsLogicFactory m_PlayerControlsLogicFactory;
    ShipMovementLogicFactory m_ShipMovementLogicFactory;
    RotatorLogicFactory m_RotatorLogicFactory;
    TurbulenceWobbleLogicFactory m_TurbulenceWobbleLogicFactory;
    AIControlsLogicFactory m_AIControlsLogicFactory;
    WeaponsControllerLogicFactory m_WeaponsControllerLogicFactory;
    ShipSpawnerLogicFactory m_ShipSpawnerLogicFactory;
};

extern LogicComponentFactories g_LogicComponentFactories;
