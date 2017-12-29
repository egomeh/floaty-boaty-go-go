#include "textasset.hpp"

GLSL *GlslAssetFactory::GetAsset(const std::string &name)
{
    if (m_LoadedAssets.count(name) != 0)
    {
        return m_LoadedAssets[name].get();
    }

    std::shared_ptr<GLSL> file = std::make_shared<GLSL>();

    m_LoadedAssets.insert(std::make_pair(name, file));

    RefreshAsset(name);

    return file.get();
}

void GlslAssetFactory::RefreshAsset(const std::string &name)
{
    auto asset = m_LoadedAssets.find(name);

    if (asset == m_LoadedAssets.end())
    {
        return;
    }

    nlohmann::json &glslAsset = (*m_AssetSubMap)[name];

    std::string path = glslAsset["path"];

    if (m_AssetDependencyTracker)
    {
        m_AssetDependencyTracker->InsertDependency<GLSL>(name, path);
    }

    std::string fileContent = m_ResourceLoader->LoadFileAsText(path);

#if SHADER_COMPILE_DEBUG
    std::stringstream debugContent;

    std::stringstream inputContent(fileContent);

    std::size_t linecounter = 0;

    std::string line;
    for (std::string line; SafeGetline(inputContent, line);)
    {
        if (line.size() > 0 && line[0] != '#')
        {
            debugContent << "#line " << linecounter << " \"" << name << "\"\n";
        }
        debugContent << line << "\n";
        ++linecounter;
    }

    asset->second->content = debugContent.str();
#else
    asset->second->content = fileContent;
#endif
}
