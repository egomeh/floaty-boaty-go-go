#include "shaderasset.hpp"

#include "serialization/jsonserializer.hpp"
#include "textasset.hpp"
#include "assetdatabase.hpp"

#include <regex>
#include <algorithm>

Shader *ShaderAssetFactory::GetAsset(const std::string &name)
{
    if (m_LoadedAssets.count(name) != 0)
    {
        return m_LoadedAssets[name].get();
    }

    std::shared_ptr<Shader> newShader = std::make_shared<Shader>(name);

    m_LoadedAssets.insert(std::make_pair(name, newShader));

    RefreshAsset(name);

    return newShader.get();
}

void ShaderAssetFactory::RefreshAsset(const std::string &name)
{
    auto &shaderToRefresh = m_LoadedAssets.find(name);

    if (shaderToRefresh == m_LoadedAssets.end())
    {
        return;
    }

    nlohmann::json &shaderAsset = (*m_AssetSubMap)[name];

    std::string path = shaderAsset["path"];

    if (m_AssetDependencyTracker)
    {
        m_AssetDependencyTracker->InsertDependency<Shader>(name, path);
    }

    std::string shaderJson = m_ResourceLoader->LoadFileAsText(path);

    ShaderImportData importData;

    JsonSerializer serializer(shaderJson);

    importData.Deserialize(serializer);

    ShaderProperties shaderProperties;

    shaderProperties.SetBlendMode(importData.m_BlendMode);
    shaderProperties.SetWriteToDepth(importData.writedepth);

    GLSL *vertexFile = m_AssetDatabase->RequestAsset<GLSL>(importData.vertex);
    GLSL *fragmentFile = m_AssetDatabase->RequestAsset<GLSL>(importData.fragment);

    if (m_AssetDependencyTracker)
    {
        std::size_t glslAssetTypeId = GetTypeID<GLSL>();

        AssetIdentifier vertexShaderIdentifier(glslAssetTypeId, importData.vertex);
        AssetIdentifier fragmenthaderIdentifier(glslAssetTypeId, importData.fragment);

        m_AssetDependencyTracker->InsertDependency<Shader>(name, vertexShaderIdentifier);
        m_AssetDependencyTracker->InsertDependency<Shader>(name, fragmenthaderIdentifier);
    }

    std::stringstream vertexShaderSource(vertexFile->content);
    std::stringstream fragmentShaderSource(fragmentFile->content);

    std::stringstream vertexShaderPreprocessed;
    std::stringstream fragmentShaderPreprocessed;

    std::string line;
    std::size_t linecounter = 0;

    std::regex includeRegex("#include <([a-zA-Z0-9\\.]+)>");
    std::smatch includeMatch;

    std::vector<std::string> includedFiles;

    bool doInlcudePass = true;

    while (doInlcudePass)
    {
        doInlcudePass = false;

        for (std::string line; SafeGetline(vertexShaderSource, line);)
        {
            if (line.size() > 0 && line[0] == '#' && std::regex_match(line, includeMatch, includeRegex))
            {
                std::string includeName = includeMatch[1];

                if (std::find(includedFiles.begin(), includedFiles.end(), includeName) == includedFiles.end())
                {
                    includedFiles.push_back(includeName);

                    GLSL *include = m_AssetDatabase->RequestAsset<GLSL>(includeName);

                    const std::string &includeContent = include->content;

                    std::stringstream includeStream(includeContent);

                    for (std::string includeLine; SafeGetline(includeStream, includeLine);)
                    {
                        vertexShaderPreprocessed << includeLine << std::endl;
                    }

                    doInlcudePass = true;
                }
            }
            else
            {
                vertexShaderPreprocessed << line << std::endl;
            }
        }

        vertexShaderSource = std::stringstream();
        vertexShaderSource << vertexShaderPreprocessed.str();
        vertexShaderPreprocessed = std::stringstream("");
    }

    if (m_AssetDependencyTracker)
    {
        std::size_t glslAssetTypeId = GetTypeID<GLSL>();

        for (const std::string &includedFile : includedFiles)
        {
            AssetIdentifier glslIdentifier(glslAssetTypeId, includedFile);
            m_AssetDependencyTracker->InsertDependency<Shader>(name, glslIdentifier);
        }
    }

    includedFiles.clear();

    doInlcudePass = true;

    while (doInlcudePass)
    {
        doInlcudePass = false;

        for (std::string line; SafeGetline(fragmentShaderSource, line);)
        {
            if (line.size() > 0 && line[0] == '#' && std::regex_match(line, includeMatch, includeRegex))
            {
                std::string includeName = includeMatch[1];

                if (std::find(includedFiles.begin(), includedFiles.end(), includeName) == includedFiles.end())
                {
                    includedFiles.push_back(includeName);

                    GLSL *include = m_AssetDatabase->RequestAsset<GLSL>(includeName);

                    const std::string &includeContent = include->content;

                    std::stringstream includeStream(includeContent);

                    for (std::string includeLine; SafeGetline(includeStream, includeLine);)
                    {
                        fragmentShaderPreprocessed << includeLine << std::endl;
                    }

                    doInlcudePass = true;
                }
            }
            else
            {
                fragmentShaderPreprocessed << line << std::endl;
            }
        }

        fragmentShaderSource = std::stringstream();
        fragmentShaderSource << fragmentShaderPreprocessed.str();
        fragmentShaderPreprocessed = std::stringstream("");
    }

    if (m_AssetDependencyTracker)
    {
        std::size_t glslAssetTypeId = GetTypeID<GLSL>();

        for (const std::string &includedFile : includedFiles)
        {
            AssetIdentifier glslIdentifier(glslAssetTypeId, includedFile);
            m_AssetDependencyTracker->InsertDependency<Shader>(name, glslIdentifier);
        }
    }

    std::string finalVertexShaderSource = m_OpenGLVersion.GetShaderVersionTag() + vertexShaderSource.str();
    std::string finalFragmentShaderSource = m_OpenGLVersion.GetShaderVersionTag() + fragmentShaderSource.str();

    std::shared_ptr<Shader> shader = std::make_shared<Shader>(name);

    shader->Compile(finalVertexShaderSource, finalFragmentShaderSource);

    if (shader->GetErrorLog().size() > 0)
    {
        DebugLog(shader->GetErrorLog());
    }

    shaderToRefresh->second->TakeProgramOwnership(shader);
    shaderToRefresh->second->SetProperties(shaderProperties);
}

void ShaderAssetFactory::SetOpenGLVersion(OpenGLVersion version)
{
    m_OpenGLVersion = version;
}

