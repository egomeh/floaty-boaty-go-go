#pragma once

#include "fontasset.hpp"
#include "assetdatabase.hpp"
#include "textureasset.hpp"
#include "cubemapasset.hpp"
#include "textasset.hpp"
#include "shaderasset.hpp"
#include "meshasset.hpp"
#include "sceneasset.hpp"
#include "materialasset.hpp"

#include "meshrendercomponent.hpp"
#include "transformcomponent.hpp"
#include "cameracomponent.hpp"
#include "spherecollidercomponent.hpp"

class AssetFactories
{
public:
    FontAssetFactory m_FontAssetFactory;
    FontTextureAssetFactory m_FontTextureAssetFactory;
    TextureAssetFactory m_TextureAssetFactory;
    CubemapAssetFactory m_CubemapAssetFactory;
    GlslAssetFactory m_GlslAssetFactory;
    ShaderAssetFactory m_ShaderAssetFactory;
    MeshAssetFactory m_MeshAssetFactory;
    SceneAssetFactory m_SceneAssetFactory;
    PrefabAssetFactory m_PrefabAssetFactory;
    MaterialAssetFactory m_MaterialAssetFactory;
};

class ComponentLoaderFactories
{
public:
    MeshRenderLoader m_MeshRenderLoader;
    TransformLoader m_TransformLoader;
    CameraLoader m_CameraLoader;
    SphereColliderLoader m_SphereColliderSystem;
};

