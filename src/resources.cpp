#include "resources.hpp"

#include <fstream>
#include <string>

#include "assetdatabase.hpp"
#include "resources.hpp"

#include "project_config.h"

#define ASSET_MAP_RESOURCE 101

void ResourceLoader::Initialize()
{
#if LOAD_ASSETS_FROM_EXE
    HRSRC resource;
    HGLOBAL resourceHandle;
    char* resourceData;
    DWORD resourceSize;

    resource = FindResource(NULL, MAKEINTRESOURCE(ASSET_MAP_RESOURCE), RT_RCDATA);
    resourceHandle = LoadResource(NULL, resource);

    resourceData = (char*)LockResource(resourceHandle);
    resourceSize = SizeofResource(NULL, resource);

    std::string assetMapping(resourceData, resourceData + resourceSize);

    nlohmann::json assetMap = nlohmann::json::parse(assetMapping);

    m_ResourceMap = assetMap["resourceIdentifiers"];
#else
#endif
}

bool ResourceLoader::HasResource(std::string filename) const
{
#if LOAD_ASSETS_FROM_EXE
    return m_ResourceMap.find(filename) != m_ResourceMap.end();
#else
    std::string filePath = ASSETS_DIR "/" + filename;

    std::ifstream fileStream(filePath.c_str());

    bool fileCouldBeRead = fileStream.good();

    fileStream.close();

    return fileCouldBeRead;
#endif
}

std::string ResourceLoader::LoadFileAsText(std::string filename) const
{
    std::vector<char> content;

    LoadFileContent(filename, content);

    return std::string(content.begin(), content.end());
}

