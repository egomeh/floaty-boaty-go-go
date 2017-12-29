#pragma once

#include "ecs.hpp"
#include "serialization/serialization.hpp"
#include "assetdatabase.hpp"

#include "Graphics/texture.hpp"

#include <map>

enum CameraClearType
{
    ClearSolidColor,
    ClearCubemap
};

namespace CameraClearKeywords
{
    static const std::string Color("color");
    static const std::string Cubemap("cubemap");
}

const std::map<std::string, CameraClearType> clearTypeKeywordMap =
{
    {CameraClearKeywords::Color, ClearSolidColor},
    {CameraClearKeywords::Cubemap, ClearCubemap}
};

class Camera: public ComponentBase
{
public:
    template<typename SerializationType>
    void Deserialize(SerializationType serializer)
    {
        std::string cleartypestring;
        std::string skyboxasset;

        // Projection variables
        DESERIALIZE(fov, serializer);
        DESERIALIZE(nearPlane, serializer);
        DESERIALIZE(farPlane, serializer);

        // Clear color variables
        DESERIALIZE(clearColor, serializer);
        DESERIALIZE(cleartypestring, serializer);
        DESERIALIZE(skyboxasset, serializer);

        // Anti aliasing variables
        DESERIALIZE(allowMsaa, serializer);
        DESERIALIZE(msaaSamples, serializer);

        clearType = StringToEnum(cleartypestring, clearTypeKeywordMap, ClearSolidColor);

        if (clearType == CameraClearType::ClearCubemap)
        {
           cubemapSkybox = assetDatabase->RequestAsset<Cubemap>(skyboxasset);
        }
    }

    float fov;
    float nearPlane;
    float farPlane;
    bool allowMsaa;
    int msaaSamples;
    glm::vec4 clearColor;
    CameraClearType clearType;
    Cubemap *cubemapSkybox;

    // Quite ugly, but just to gets things going
    AssetDatabase *assetDatabase;
};

class CameraSystem : public ComponentSystem<Camera>
{
public:
    CameraSystem();

    // NOTE: There can only be one camera at the time.
    // Thus `Create` will always give a handle to the one and
    // only camera that exists.
    bool Create(ComponentHandle &handle);

    Camera *GetCamera();

    Camera *GetComponent(ComponentHandle handle);

    void DestroyComponent(ComponentHandle handle) override;

    void SetAssetDatabase(AssetDatabase *assetDatabase);

private:
    Camera m_MainCamera;
    AssetDatabase *m_AssetDatabase;
};
