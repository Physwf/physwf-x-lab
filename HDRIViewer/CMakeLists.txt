file(GLOB_RECURSE DIR_SRC
    "./*.cpp"
    "./*.c"
    "./*.h"
)
add_executable(HDRIViewer ${DIR_SRC})

add_definitions(-DUNICODE -D_UNICODE)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:Console")

SET(EXECUTABLE_OUTPUT_PATH  ${CMAKE_BINARY_DIR})