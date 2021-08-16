file(GLOB_RECURSE DIR_SRC
    "./*.cpp"
    "./*.c"
    "./*.h"
)
add_library(x4 SHARED ${DIR_SRC})

add_definitions(-DUNICODE -D_UNICODE)

SET(LIBRARY_OUTPUT_PATH  ${CMAKE_BINARY_DIR})

include(GenerateExportHeader)
generate_export_header(x4 
						BASE_NAME X4
						EXPORT_MACRO_NAME X4_API
						EXPORT_FILE_NAME ${PROJECT_SOURCE_DIR}/X4/X4.h
						)