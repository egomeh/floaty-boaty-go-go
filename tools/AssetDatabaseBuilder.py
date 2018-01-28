import os
import re
import sys
import json
import hashlib

from AssetTypes import *

CMakeTemplate = """
set(ASSET_FILES "")

if (${{PACK_ASSETS}})
{0}
message(${{ASSET_FILES}})
endif(${{PACK_ASSETS}})

set(LOGIC_SOURCE
{1}
)

add_library(Logic ${{LOGIC_SOURCE}})

set(ASSET_FILES "${{ASSET_FILES}}" PARENT_SCOPE)

set(SHADER_SOURCE_FILES
{2}
)

add_custom_target(Shaders ALL SOURCES ${{SHADER_SOURCE_FILES}})

foreach(source IN LISTS LOGIC_SOURCE)
    get_filename_component(source_path "${{source}}" PATH)
    string(REPLACE "/" "\\\\" source_path_msvc "${{source_path}}")
    source_group("${{source_path_msvc}}" FILES "${{source}}")
endforeach()

foreach(source IN LISTS SHADER_SOURCE_FILES)
    get_filename_component(source_path "${{source}}" PATH)
    string(REPLACE "/" "\\\\" source_path_msvc "${{source_path}}")
    source_group("${{source_path_msvc}}" FILES "${{source}}")
endforeach()
"""

resourceHeader = """
#pragma once

#define CUSTOM_FILE_TYPE 256
{0}
"""

logicSystemHeader = """#pragma once

#include "logic.hpp"
#include "assetdatabase.hpp"

{0}

void RegisterLogicFactories(LogicSystem &logicSystem);

{1}
struct LogicComponentFactories
{{
{2}
}};

extern LogicComponentFactories g_LogicComponentFactories;
"""

logicSystemImplementation = """#include "logicfactories.hpp"
#include "serialization/jsonserializer.hpp"

{1}
LogicComponentFactories g_LogicComponentFactories;

void RegisterLogicFactories(LogicSystem &logicSystem)
{{
{0}
}}
"""

logicFactoryHeaderTemplate = """class {name}LogicFactory : public LogicComponentFactory<{name}>
{{
public:
    {name}LogicFactory() : LogicComponentFactory<{name}>()
    {{
    }}

    {name} *CreatePointer() override;
    {name} *CreatePointer(const nlohmann::json &serializedComponent) override;
}};
"""

logicFactoryImplementationTemplate = """{name} *{name}LogicFactory::CreatePointer()
{{
    return new {name}();
}}

{name} *{name}LogicFactory::CreatePointer(const nlohmann::json &serializedComponent)
{{
    {name}* component = new {name}();

    JsonSerializer serializer(serializedComponent);

    component->Deserialize(serializer);

    return component;
}}
"""

def WriteIfChanged(content, filename):
    if not os.path.exists(filename):
        print "{0} changed!".format(filename)
        with open(filename, "w") as f:
            f.write(content)
            return

    fileContent = ""

    with open(filename, "r") as f:
        fileContent = f.read()


    if content == fileContent:
        return

    with open(filename, "w") as f:
        print "{0} changed!".format(filename)
        f.write(content)


def CMakeAddFile(filename):
    return 'set(ASSET_FILES ${{ASSET_FILES}} "${{PROJECT_SOURCE_DIR}}/assets/{0}")'.format(filename)

def CMakeAddFileWithName(filename, listName):
    return 'set({0} ${{{0}}} "${{PROJECT_SOURCE_DIR}}/assets/{1}")'.format(listName, filename)

def WriteMetaFile(assetFile, assetType):
    metafile = assetFile + ".meta"
    jsonData = assetType.GetJsonData()
    with open(metafile, "w") as file:
        file.write(jsonData)

def GetDefineName(assetType, asset):
    return re.sub(r'[^\w\s]','',("IDR_" + assetType + "_" +  asset).upper())

def AddMissingMetaData(assetFile, assetType):
    metafile = assetFile + ".meta"
    with open(metafile, "r") as file:
        jsonData = json.loads(file.read())
        assetType.ReadMetaData(jsonData)
        WriteMetaFile(assetFile, assetType)

def FileHasMeta(filename):
    metaFile = filename + ".meta"
    return os.path.isfile(metaFile)

def GetFileExtension(filename):
    tokens = filename.split(".")

    if len(tokens) < 2:
        return ""

    return tokens[-1]

def BuildAssetMapping(directory, assets):
    assetMapFileName = os.path.join(directory, "assetmap.json")

    with open(assetMapFileName, "w") as f:
        f.write(json.dumps(assets, indent=4, sort_keys=True))

def AddAsset(assetType, name, path, hasMeta, assets):
    if not assetType in assets:
        assets[assetType] = {}

    assetRecord = {}
    assetRecord["path"] = path
    assetRecord["hasMeta"] = hasMeta

    assets[assetType][name] = assetRecord

def BuildCMakeLists(directory, assets, logicComponents):
    assetMapFileName = os.path.join(directory, "CMakeLists.txt")

    addFilesString = " " * 4 + CMakeAddFile("assetmap.json")

    for assetType in assets:
        for asset in assets[assetType]:
            addFilesString += "\n" + " " * 4 + CMakeAddFile(assets[assetType][asset]["path"])
            if assets[assetType][asset]["hasMeta"]:
                addFilesString += "\n" + " " * 4 + CMakeAddFile(str(assets[assetType][asset]["path"]) + ".meta")

    shaderSources = []

    for shader in assets["shader"]:
        shaderSources.append(assets["shader"][shader]["path"])

    for glsl in assets["glsl"]:
        shaderSources.append(assets["glsl"][glsl]["path"])

    shaderSourceString = "\n".join([4 * " " + path for path in shaderSources])

    logicComponentList = []

    #logicComponentList.append(CMakeAddFileWithName("logicsystem.hpp", "LOGIC_SOURCE"))
    #logicComponentList.append(CMakeAddFileWithName("logicsystem.cpp", "LOGIC_SOURCE"))

    for logicComponent in logicComponents:
        (assetType, relativePath, hasCpp) = logicComponents[logicComponent]

        pathPrefix = ""
        if not len(relativePath) == 0:
            pathPrefix = relativePath + "/"

        logicComponentList.append(pathPrefix + logicComponent + ".hpp")

        if hasCpp:
            logicComponentList.append(pathPrefix + logicComponent + ".cpp")

    logicSourceString = "\n    ".join([path for path in logicComponentList])

    cmakeContent = CMakeTemplate.format(addFilesString, logicSourceString, shaderSourceString)

    WriteIfChanged(cmakeContent, assetMapFileName)

def BuildLogicSystem(directory, assets, logicComponents):
    headerFilename = os.path.join(directory, "logicfactories.hpp")
    implementationFilename = os.path.join(directory, "logicfactories.cpp")

    logicFactoryHeaders = []
    logicFactoryImplementation = []
    logicIncludes = []
    logicRegistrations = []
    logicFactoryStructMembers = []

    for logicComponent in logicComponents:
        if logicComponent == "logicfactories":
            continue

        (assetType, relativePath, hasCpp) = logicComponents[logicComponent]

        factoryHeader = logicFactoryHeaderTemplate.format(name = logicComponent)
        factoryImplementation = logicFactoryImplementationTemplate.format(name = logicComponent)

        logicFactoryHeaders.append(factoryHeader)
        logicFactoryImplementation.append(factoryImplementation)
        logicIncludes.append('#include "{0}/{1}.hpp"'.format(relativePath, logicComponent))
        logicRegistrations.append('    logicSystem.RegisterFactory<{0}>("{0}", &g_LogicComponentFactories.m_{0}LogicFactory);'.format(logicComponent))
        logicFactoryStructMembers.append('    {0}LogicFactory m_{0}LogicFactory;'.format(logicComponent))


    headerContent = logicSystemHeader.format("\n".join(logicIncludes), "\n".join(logicFactoryHeaders), "\n".join(logicFactoryStructMembers))
    implementation = logicSystemImplementation.format("\n".join(logicRegistrations), "\n".join(logicFactoryImplementation))

    WriteIfChanged(headerContent, headerFilename)

    WriteIfChanged(implementation,  implementationFilename)

def BuildResource(directory, assets):
    headerFilename = os.path.join(directory, "resource.h")
    rcFilename = os.path.join(directory, "resource.rc")

    resourceIdentifiers = {"assetmap.json": 101}

    counter = 101

    settingsFile = directory + "settings.json"

    icon = None

    if os.path.isfile(settingsFile):
        with open(settingsFile) as settingsFileHandle:
            settingsRawContent = settingsFileHandle.read()
            settingsObject = json.loads(settingsRawContent)

            if "icon" in settingsObject:
                icon = settingsObject["icon"]

    headerDefines = "\n#define IDR_ASSET_MAP {0}\n".format(counter)
    resources = "#include \"resource.h\"\n\nIDR_ASSET_MAP RCDATA \"{0}\"".format("assetmap.json")

    counter += 1

    if icon != None:
        headerDefines += "\n#define IDR_ICON_1 {0}\n".format(counter)
        resources += "\nIDI_ICON_1 ICON \"{0}\"\n".format(icon)

        counter += 1

    for assetType in assets:
        for asset in assets[assetType]:
            defineName = GetDefineName(assetType, asset)

            headerDefines += "\n" + "#define " + defineName + " {0}".format(counter)

            resources += "\n" + defineName + " RCDATA \"" + assets[assetType][asset]["path"] + "\""

            resourceIdentifiers[assets[assetType][asset]["path"]] = counter

            counter += 1

            if assets[assetType][asset]["hasMeta"]:
                headerDefines += "\n" + "#define " + GetDefineName(assetType, asset + "_META") + " {0}".format(counter)

                resources += "\n" + GetDefineName(assetType, asset + "_META") + " RCDATA \"" + assets[assetType][asset]["path"] + ".meta" + "\""

                resourceIdentifiers[assets[assetType][asset]["path"] + ".meta"] = counter

                counter += 1

    headerDefines = resourceHeader.format(headerDefines)

    WriteIfChanged(headerDefines, headerFilename)

    WriteIfChanged(resources, rcFilename)

    assets["resourceIdentifiers"] = resourceIdentifiers

def CheckDirectory((assets, logicComponents, path), directory, files):
    for file in files:
        if file.endswith("assetmap.json"):
            continue

        fullPath = os.path.join(directory, file).replace("\\","/")

        if os.path.isfile(fullPath):
            assetFileExtension = GetFileExtension(fullPath)
            relativePath = os.path.relpath(fullPath, path).replace("\\", "/")

            fullDir = os.path.abspath(os.path.dirname(fullPath))
            relativeDir = os.path.dirname(relativePath)

            assetType = GetAssetType(assetFileExtension)

            if assetType is None:
                continue

            basename = os.path.basename(file)

            for extension in assetType.acceptedFileExtensions:
                fileExtension = "." + extension

                if basename.endswith(fileExtension):
                    basename = basename[:-len(fileExtension)]
                    break

            if assetType.isLogic:
                hasCpp = os.path.isfile(os.path.join(fullDir, basename + ".cpp"))
                logicComponents[basename] = (assetType, relativeDir, hasCpp)
                continue

            if not assetType.requiresMeta:
                if not assetType.isJson:
                    AddAsset(assetType.metaData["type"], basename, relativePath, False, assets)
                    continue

                with open(fullPath, "r") as f:
                    jsonData = json.loads(f.read())
                    assetType.ReadMetaData(jsonData)

                AddAsset(assetType.metaData["type"], assetType.metaData["name"], relativePath, False, assets)
                continue

            assetType.metaData["name"] = basename

            if not FileHasMeta(fullPath):
                print "Writng new meta file for " + fullPath
                WriteMetaFile(fullPath, assetType)
            else:
                AddMissingMetaData(fullPath, assetType)

            AddAsset(assetType.metaData["type"], assetType.metaData["name"], relativePath, True, assets)

def BuildDatabase(path):
    assets = {}
    logicComponents = {}
    os.path.walk(path, CheckDirectory, (assets, logicComponents, path))
    BuildCMakeLists(path, assets, logicComponents)
    BuildLogicSystem(path, assets, logicComponents)
    BuildResource(path, assets)
    BuildAssetMapping(path, assets)

if __name__ == "__main__":
    if len(sys.argv) < 2:
        BuildDatabase("./../assets/")
    else:
        BuildDatabase(sys.argv[1])

