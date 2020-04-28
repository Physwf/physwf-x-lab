set(DIR_PROJECT "${CMAKE_CURRENT_SOURCE_DIR}/")
file(
    GLOB_RECURSE _source_list 
    LIST_DIRECTORIES false
    "${DIR_PROJECT}/*.cpp"
    "${DIR_PROJECT}/*.h"
    "${DIR_PROJECT}/*.inl"
)

foreach(_source IN ITEMS ${_source_list})
    get_filename_component(_source_path "${_source}" PATH)
	file(RELATIVE_PATH _source_path_rel "${DIR_PROJECT}" "${_source_path}")
    string(REPLACE "/" "\\" _group_path "${_source_path_rel}")
    source_group("${_group_path}" FILES "${_source}")
endforeach()

add_library(libx4 SHARED ${_source_list})
set_target_properties(libx4 PROPERTIES FOLDER "Libraries")
target_include_directories(libx4 PRIVATE "./Include")
target_include_directories(libx4 PRIVATE "./Source")
target_include_directories(libx4 PRIVATE "../x5/Include")

SET(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

include(GenerateExportHeader)
generate_export_header(libx4 
						BASE_NAME X4
						EXPORT_MACRO_NAME X4_API
						EXPORT_FILE_NAME ${PROJECT_SOURCE_DIR}/x4/Include/X4.h
						)

if (CMAKE_SYSTEM_NAME MATCHES "Linux")
	message(status " current platform: Linux ")
elseif(CMAKE_SYSTEM_NAME MATCHES "Windows")
	message(status " current platform: Windows")
	message(status " CMAKE_SYSTEM_VERSION=${CMAKE_SYSTEM_VERSION}")
	message(status " CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION=${CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION}")
	set(CMAKE_SYSTEM_VERSION,10.0.17134.0)
	add_definitions(-DPLATFORM_WINDOWS)
	add_definitions(-DWITH_EDITOR=0)
	add_definitions(-DWITH_ENGINE=1)
	add_definitions(-DWITH_PLUGIN_SUPPORT=0)
	add_definitions(-DWITH_UNREAL_DEVELOPER_TOOLS=0)
	add_definitions(-DIS_MONOLITHIC=0)
	add_definitions(-DIS_PROGRAM=0)
	add_definitions(-DUE_BUILD_MINIMAL=1)
	add_definitions(-DUE_BUILD_DEBUG)
	add_definitions(-DUNICODE -D_UNICODE)
else()
	message(status "other platform: ${CMAKE_SYSTEM_NAME}")
endif()