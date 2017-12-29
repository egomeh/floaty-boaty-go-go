#pragma once

#include <windows.h>
#include <string>
#include <vector>
#include <fstream>

#include "project_config.h"

#include "json.hpp"

class ResourceLoader
{
public:
    void Initialize();

    bool HasResource(std::string filename) const;

    template<typename T>
    void LoadFileContent(std::string filename, std::vector<T> &data) const;

    std::string LoadFileAsText(std::string filename) const;

#if LOAD_ASSETS_FROM_EXE
    nlohmann::json m_ResourceMap;
#endif
};

template<typename T>
inline void ResourceLoader::LoadFileContent(std::string filename, std::vector<T> &data) const
{
    data.clear();

#if LOAD_ASSETS_FROM_EXE
    int assetIdentifierId = m_ResourceMap[filename];

    HRSRC resource;
    HGLOBAL resourceHandle;
    T* resourceData;
    DWORD resourceSize;

    resource = FindResource(NULL, MAKEINTRESOURCE(assetIdentifierId), RT_RCDATA);
    resourceHandle = LoadResource(NULL, resource);

    resourceData = (T*)LockResource(resourceHandle);
    resourceSize = SizeofResource(NULL, resource);

    data.assign(resourceData, resourceData + resourceSize);
#else
    std::string filePath = ASSETS_DIR "/" + filename;

    std::ifstream fileStream(filePath, std::ios::binary);

    if (!fileStream.good())
    {
        std::stringstream errorMessage;
        errorMessage << "Could not open " << filePath;
        throw std::runtime_error(errorMessage.str());
    }

    std::istreambuf_iterator<char> it1(fileStream);
    std::istreambuf_iterator<char> it2;

    data.insert(data.end(),  it1, it2);
#endif
}
