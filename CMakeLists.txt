cmake_minimum_required(VERSION 3.1)

project (FloatyBoatyGoGo)

set(EXE_NAME "FloatyBoatyGoGo")

set(APP_NAME "FloatyBoatyGoGo")

set(CONFIG_DIR ${PROJECT_BINARY_DIR}/configuration)

set_property( GLOBAL PROPERTY USE_FOLDERS ON)

set(SHADER_PACKAGE_FILE "${CONFIG_DIR}/shaders.hpp")
set(ShaderDir ${PROJECT_SOURCE_DIR}/assets/shaders)
set(AssetDir ${PROJECT_SOURCE_DIR}/assets)
set(ConfigDir ${CONFIG_DIR})

configure_file (
  "${PROJECT_SOURCE_DIR}/project_config.h.in"
  "${PROJECT_BINARY_DIR}/configuration/project_config.h"
)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

set(SHADER_DEBUG_INFO OFF CACHE BOOL "Compile shaders with debug information")
set(DO_ONLY_GUI OFF CACHE BOOL "Ommit the windows console")
set(CHECK_OPENGL_ERRORS OFF CACHE BOOL "Do Check for OpenGL error states")
set(PACK_ASSETS OFF CACHE BOOL "Packl assets into statically linked data")
set(MULTICORE_COMPILE OFF CACHE BOOL "Compile with multiple cores")

if (${SHADER_DEBUG_INFO})
	add_definitions(-DSHADER_COMPILE_DEBUG)
endif (${SHADER_DEBUG_INFO})

if (${CHECK_OPENGL_ERRORS})
	add_definitions(-DCHECK_OPENGL_ERRORS)
endif (${CHECK_OPENGL_ERRORS})

include_directories(lib/glm)
include_directories(lib/json/src)
include_directories(lib/stb)
include_directories(lib/glew-cmake/include)
include_directories(lib/assimp/include)
include_directories(lib/assimp/build/include)
include_directories(lib/delaunay-triangulation)
include_directories(assets)

set(GLEW_LIBRARY_DIRS "${PROJECT_SOURCE_DIR}/lib/glew-cmake/build/lib/")
set(ASSIMP_LIBRARY_DIRS "${PROJECT_SOURCE_DIR}/lib/assimp/build/code/")
set(ASSIMP_IRRXML_LIBRARY_DIRS "${PROJECT_SOURCE_DIR}/lib/assimp/build/contrib/irrXML/")
set(ASSIMP_ZLIB_LIBRARY_DIRS "${PROJECT_SOURCE_DIR}/lib/assimp/build/contrib/zlib/")

link_directories(${GLEW_LIBRARY_DIRS})
link_directories(${ASSIMP_LIBRARY_DIRS})
link_directories(${ASSIMP_IRRXML_LIBRARY_DIRS})
link_directories(${ASSIMP_ZLIB_LIBRARY_DIRS})

find_package(OpenGL REQUIRED)

include_directories(${PROJECT_BINARY_DIR}/configuration)
include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${OPENGL_INCLUDE_DIR})

set(OPTIONS "")

if(WIN32)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
endif(WIN32)

if(MSVC AND ${DO_ONLY_GUI})
	set(OPTIONS ${OPTIONS} WIN32)
	add_definitions(-DGUIONLY)
endif(MSVC AND ${DO_ONLY_GUI})

set(CMAKE_CXX_FLAGS_RELEASE "/MT")

if (${MULTICORE_COMPILE})
	add_definitions(${CMAKE_CXX_FLAGS} "/MP")
endif(${MULTICORE_COMPILE})

if (${PACK_ASSETS})
	add_definitions(-DLOAD_ASSETS_FROM_EXE)
	SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MP")
	set(RESOURCE_FILES ${PROJECT_SOURCE_DIR}/assets/resource.h ${PROJECT_SOURCE_DIR}/assets/resource.rc)
endif(${PACK_ASSETS})

add_subdirectory(assets)
add_subdirectory(src)

add_custom_target(
    BuildAssetDatabase ALL
    COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/tools/AssetDatabaseBuilder.py
)

add_custom_target(
    MakeNewLogicComponent
    COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/tools/CreateNewLogicComponent.py
)

