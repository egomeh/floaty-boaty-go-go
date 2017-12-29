#pragma once

#include "assetdatabase.hpp"
#include "graphics/meshrenderer.hpp"
#include "ecs.hpp"
#include "graphics/camera.hpp"

class CameraLoader : public ComponentLoader<Camera>
{
public:
    TypedComponentHandle LoadComponent(const nlohmann::json &serializedComponent) override;
    TypedComponentHandle LoadComponent(ComponentTypeId) override;
    ComponentBase *GetAbstractComponent(ComponentHandle handle);

    void SetCameraSystem(CameraSystem *cameraSystem);

private:
    CameraSystem *m_CameraSystem;
};
