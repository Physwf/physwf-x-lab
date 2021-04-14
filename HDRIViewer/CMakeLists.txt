file(GLOB_RECURSE DIR_SRC
    "./*.cpp"
    "./*.c"
    "./*.h"
)
add_executable(HDRIViewer ${DIR_SRC})
target_link_libraries(HDRIViewer libx9)
target_include_directories(HDRIViewer PRIVATE "../x9")

add_definitions(-DUNICODE -D_UNICODE)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS")

SET(EXECUTABLE_OUTPUT_PATH  ${CMAKE_BINARY_DIR})