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
}
