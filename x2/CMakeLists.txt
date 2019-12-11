file(GLOB_RECURSE DIR_SRC
    "./*.cpp"
    "./*.h"
)
add_library(libx2 SHARED ${DIR_SRC})
set_target_properties(libx2 PROPERTIES FOLDER "Libraries")

SET(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})
MESSAGE(STATUS "PROJECT_SOURCE_DIR is ${PROJECT_SOURCE_DIR}")
MESSAGE(STATUS "CMAKE_BINARY_DIR is ${CMAKE_BINARY_DIR}")

include(GenerateExportHeader)
generate_export_header(libx2 
						BASE_NAME NAIL
						EXPORT_MACRO_NAME NAIL_API
						EXPORT_FILE_NAME ./NAIL.h
						)