file(GLOB_RECURSE DIR_SRC
    "./*.cpp"
    "./*.c"
    "./*.h"
)
add_library(libx9 SHARED ${DIR_SRC})

add_definitions(-DUNICODE -D_UNICODE)

SET(EXECUTABLE_OUTPUT_PATH  ${CMAKE_BINARY_DIR})

include(GenerateExportHeader)
generate_export_header(libx9 
						BASE_NAME X9
						EXPORT_MACRO_NAME X9_API
						EXPORT_FILE_NAME ${PROJECT_SOURCE_DIR}/x9/X9.h
						)