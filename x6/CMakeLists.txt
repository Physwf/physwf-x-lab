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

add_executable(JEngine ${_source_list})
target_include_directories(JEngine PRIVATE "./Include")
target_include_directories(JEngine PRIVATE "./Source")
target_include_directories(JEngine PRIVATE "../x0/Include")
target_include_directories(JEngine PRIVATE "../x5/Include")
target_link_libraries(JEngine libx0)
target_link_libraries(JEngine libx5)

SET(EXECUTABLE_OUTPUT_PATH  ${CMAKE_BINARY_DIR})

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS")

if (CMAKE_SYSTEM_NAME MATCHES "Linux")
	message(status " current platform: Linux ")
elseif(CMAKE_SYSTEM_NAME MATCHES "Windows")
	message(status " current platform: Windows")
	add_definitions(-DPLATFORM_WINDOWS)
else()
	message(status "other platform: ${CMAKE_SYSTEM_NAME}")
endif()