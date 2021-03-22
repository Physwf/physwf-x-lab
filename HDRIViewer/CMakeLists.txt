file(GLOB_RECURSE DIR_SRC
    "./*.cpp"
    "./*.c"
    "./*.h"
)
add_executable(HDRIViewer ${DIR_SRC})