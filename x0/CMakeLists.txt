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

add_library(libx0 SHARED ${_source_list})
set_target_properties(libx0 PROPERTIES FOLDER "Libraries")
target_include_directories(libx0 PRIVATE "./Include")
target_include_directories(libx0 PRIVATE "./Source")

SET(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

message(status "PROJECT_SOURCE_DIR=${PROJECT_SOURCE_DIR}")
include(GenerateExportHeader)
generate_export_header(libx0 
						BASE_NAME X0
						EXPORT_MACRO_NAME X0_API
						EXPORT_FILE_NAME ${PROJECT_SOURCE_DIR}/x0/Include/X0.h
						)

if (CMAKE_SYSTEM_NAME MATCHES "Linux")
	message(status " current platform: Linux ")
elseif(CMAKE_SYSTEM_NAME MATCHES "Windows")
	message(status " current platform: Windows")
	message(status " CMAKE_SYSTEM_VERSION=${CMAKE_SYSTEM_VERSION}")
	message(status " CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION=${CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION}")
	set(CMAKE_SYSTEM_VERSION,10.0.17134.0)
	add_definitions(-DPLATFORM_WINDOWS)
	add_definitions(-DUNICODE -D_UNICODE)
else()
	message(status "other platform: ${CMAKE_SYSTEM_NAME}")
endif()