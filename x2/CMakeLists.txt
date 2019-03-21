file(GLOB_RECURSE DIR_SRC
    "./*.cpp"
    "./*.h"
)
add_library(libx2 SHARED ${DIR_SRC})
set_target_properties(libx2 PROPERTIES FOLDER "Libraries")
