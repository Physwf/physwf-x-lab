#[[set(DIR_PROJECT "./")
file(GLOB_RECURSE DIR_SRC
    "${DIR_PROJECT}/Source/*.cpp"
    "${DIR_PROJECT}/Source/*.h"
)
file(GLOB_RECURSE DIR_INC
    "${DIR_PROJECT}/Include/*.h"
)]]#

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

add_library(libx5 SHARED ${_source_list})
set_target_properties(libx5 PROPERTIES FOLDER "Libraries")
target_include_directories(libx5 PRIVATE "./Include")
target_include_directories(libx5 PRIVATE "./Source")
target_include_directories(libx5 PRIVATE "../x0/Include")
target_link_libraries(libx5 libx0)



if (CMAKE_SYSTEM_NAME MATCHES "Linux")
	message(status " current platform: Linux ")
elseif(CMAKE_SYSTEM_NAME MATCHES "Windows")
	message(status " current platform: Windows")
	add_definitions(-DPLATFORM_WINDOWS)
	target_link_libraries (libx5 "d3d11.lib")
else()
	message(status "other platform: ${CMAKE_SYSTEM_NAME}")
endif()