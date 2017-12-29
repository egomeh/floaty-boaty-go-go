#pragma once

#include <memory>
#include <unordered_map>
#include <map>

#include "assetdatabase.hpp"
#include "graphics/mesh.hpp"
#include "serialization/serialization.hpp"

enum NormalSource
{
    Import,
    Smooth,
    Flat
};

const std::map<std::string, NormalSource> meshNormalSourceMap =
{
    {"import", NormalSource::Import},
    {"smooth", NormalSource::Smooth},
    {"flat", NormalSource::Flat},
};

class MeshAssetFactory : public AssetFactory<Mesh>
{
public:
    Mesh *GetAsset(const std::string &name) override;

private:
    std::unordered_map<std::string, std::shared_ptr<Mesh>> m_LoadedAssets;
};
