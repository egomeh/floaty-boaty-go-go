#include "fontasset.hpp"

#include "serialization/jsonserializer.hpp"

#include "graphics/font.hpp"
#include "graphics/graphicsutils.hpp"

#include <algorithm>
#include <vector>

#include "util.hpp"

Font *FontAssetFactory::GetAsset(const std::string &name)
{
    if (m_LoadedAssets.count(name))
    {
        return m_LoadedAssets[name].get();
    }

    std::shared_ptr<Font> font = std::make_shared<Font>();

    m_LoadedAssets.insert(std::make_pair(name, font));

    RefreshAsset(name);

    return font.get();
}

void FontAssetFactory::RefreshAsset(const std::string &name)
{
    auto asset = m_LoadedAssets.find(name);

    if (asset == m_LoadedAssets.end())
    {
        return;
    }

    nlohmann::json &fontAsset = (*m_AssetSubMap)[name];

    std::string path = fontAsset["path"];

    if (m_AssetDependencyTracker)
    {
        m_AssetDependencyTracker->InsertDependency<Font>(name, path);
    }

    std::vector<uint8_t> rawFontData;

    m_ResourceLoader->LoadFileContent(path, rawFontData);

    asset->second.get()->SetRawFontData(rawFontData);
}

FontTexture *FontTextureAssetFactory::GetAsset(const std::string &name)
{
    if (m_LoadedAssets.count(name))
    {
        return m_LoadedAssets[name].get();
    }

    std::shared_ptr<FontTexture> texture = std::make_shared<FontTexture>();

    m_LoadedAssets.insert(std::make_pair(name, texture));

    RefreshAsset(name);

    return texture.get();
}

void FontTextureAssetFactory::RefreshAsset(const std::string &name)
{
    auto asset = m_LoadedAssets.find(name);

    if (asset == m_LoadedAssets.end())
    {
        return;
    }
}
