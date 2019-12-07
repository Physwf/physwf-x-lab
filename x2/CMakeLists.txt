file(GLOB_RECURSE DIR_SRC
    "./*.cpp"
    "./*.h"
)
add_library(libx2 SHARED ${DIR_SRC})
set_target_properties(libx2 PROPERTIES FOLDER "Libraries")
CHECK_CXX_COMPILER_FLAG("/std:c++17" _cpp_17_flag_supported)
add_compile_options("/std:c++17")
include(GenerateExportHeader)
generate_export_header(libx2 
						BASE_NAME NAIL
						EXPORT_MACRO_NAME NAIL_API
						EXPORT_FILE_NAME ./NAIL.h
						)