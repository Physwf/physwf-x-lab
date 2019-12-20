set(DIR_PROJECT "${CMAKE_CURRENT_SOURCE_DIR}/")
file(
    GLOB_RECURSE _source_list 
    LIST_DIRECTORIES false
    "${DIR_PROJECT}/*.cpp"
    "${DIR_PROJECT}/*.h"
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
	add_definitions(-DPLATFORM_WINDOWS)
else()
	message(status "other platform: ${CMAKE_SYSTEM_NAME}")
endif()