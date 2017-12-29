
macro (add_sources)
    file (RELATIVE_PATH _relPath "${PROJECT_SOURCE_DIR}/src" "${CMAKE_CURRENT_SOURCE_DIR}")
    foreach (_src ${ARGN})
        if (_relPath)
            list (APPEND SOURCE_FILES "${_relPath}/${_src}")
        else()
            list (APPEND SOURCE_FILES "${_src}")
        endif()
    endforeach()
    if (_relPath)
        # propagate SOURCE_FILES to parent directory
        set (SOURCE_FILES ${SOURCE_FILES} PARENT_SCOPE)
    endif()
endmacro()

if (${PACK_ASSETS})
message(${ASSET_FILES})
endif (${PACK_ASSETS})

set(APPLICATION_SOURCE_FILES
	main.cpp
	util.hpp
	util.cpp
	input.hpp
	input.cpp
	resources.hpp
	resources.cpp
	folderwatcher.hpp
	folderwatcher.cpp
	application.hpp
	application.cpp
	includeglew.hpp
	definitions.hpp
)

set(ENGINE_SOURCE_FILES
	ecs.hpp
	ecs.cpp
	scene.hpp
	scene.cpp
	logic.hpp
	logic.cpp
	transform.hpp
	transform.cpp
	assetdatabase.hpp
    assetdatabase.cpp
)

set(SOURCE_FILES
	${APPLICATION_SOURCE_FILES}
	${ENGINE_SOURCE_FILES}
)

add_subdirectory(Graphics)
add_subdirectory(Physics)
add_subdirectory(Serialization)
add_subdirectory(AssetLoading)

if (WIN32)
	add_executable(${EXE_NAME} ${OPTIONS}
		${SOURCE_FILES}
		${GRAPHICS_SOURCE_FILES}
		${RESOURCE_FILES}
	)

	target_link_libraries(
	    ${EXE_NAME}
		${OPENGL_LIBRARIES}
		Logic
		)

		target_link_libraries(
			${EXE_NAME}
			debug Debug/glewd
			optimized Release/glew
		)


		target_link_libraries(
			${EXE_NAME}
			debug Debug/assimp Debug/IrrXML Debug/zlibstaticd
			optimized Release/assimp Release/IrrXML Release/zlibstatic
		)

		set_target_properties(${EXE_NAME} PROPERTIES RESOURCE "${ASSET_FILES}")
endif (WIN32)

source_group("Application" FILES ${APPLICATION_SOURCE_FILES})
source_group("Engine" FILES ${ENGINE_SOURCE_FILES})

foreach(source IN LISTS SOURCE_FILES)
    get_filename_component(source_path "${source}" PATH)
    string(REPLACE "/" "\\" source_path_msvc "${source_path}")
    source_group("${source_path_msvc}" FILES "${source}")
endforeach()
