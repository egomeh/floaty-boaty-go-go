#pragma once

#include "json.hpp"
#include "ecs.hpp"
#include "assetdatabase.hpp"

class SceneData
{
public:
    nlohmann::json entities;
};


class Prefab
{
public:
    nlohmann::json content;
};

void LoadSceneContent(SceneData &scene, EntityDatabase &entityDatabase, ComponentLoaderSystem &componentLoader);

EntityHandle InstantiatePrefab(Prefab &prefab, EntityDatabase &entityDatabase, ComponentLoaderSystem &componentLoader);

