file(GLOB_RECURSE DIR_SRC
    "./*.cpp"
    "./*.c"
    "./*.h"
)
add_library(x5 SHARED ${DIR_SRC})

add_definitions(-DUNICODE -D_UNICODE)

SET(LIBRARY_OUTPUT_PATH  ${CMAKE_BINARY_DIR})

include(GenerateExportHeader)
generate_export_header(x5 
						BASE_NAME X5
						EXPORT_MACRO_NAME X5_API
						EXPORT_FILE_NAME ${PROJECT_SOURCE_DIR}/X5/X5.h
						)