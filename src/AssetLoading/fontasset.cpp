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

    nlohmann::json &fontTextureAsset = (*m_AssetSubMap)[name];

    std::string path = fontTextureAsset["path"];

    // If asset dependency tracker is present, add file dependency
    if (m_AssetDependencyTracker)
    {
        m_AssetDependencyTracker->InsertDependency<FontTexture>(path, name);
    }

    std::string serializedFontTexture = m_ResourceLoader->LoadFileAsText(path);

    nlohmann::json fontTextureJson;

    try
    {
        fontTextureJson = nlohmann::json::parse(serializedFontTexture);
    }
    catch (std::exception &ex)
    {
        DebugLog(ex.what());
        return;
    }

    unsigned int size = fontTextureJson["size"];
    std::string ttfSource = fontTextureJson["source"];
    float fontSize = 16.f;

    if (fontTextureJson.count("fontsize") != 0)
    {
        fontSize = fontTextureJson["fontsize"];
    }

    // Insert a dependency on the ttf source
    if (m_AssetDependencyTracker)
    {
        AssetIdentifier fontSourceIdentifier(GetTypeID<Font>(), ttfSource);
        m_AssetDependencyTracker->InsertDependency<FontTexture>(name, fontSourceIdentifier);
    }

    Font *ttf = m_AssetDatabase->RequestAsset<Font>(ttfSource);

    if (!ttf)
    {
        DebugLog("Source ttf not found");
        return;
    }

    FontTexture *storedTexture = (*asset).second.get();

    storedTexture->GenerateFontTexture(ttf, size, fontSize, FontTextureType::Raster);
}
