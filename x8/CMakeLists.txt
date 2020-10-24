file(GLOB_RECURSE DIR_SRC
    "./*.cpp"
    "./*.h"
)
add_executable(x8 ${DIR_SRC})

target_link_libraries (x8 "d3d11.lib")
target_link_libraries (x8 "dxgi.lib")
target_link_libraries (x8 "D3DCompiler.lib")
target_link_libraries (x8 "dxguid.lib")

add_definitions(-DUNICODE -D_UNICODE)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS")

#copy $(SolutionDir)..\x8\Test\*.hlsl  $(TargetDir)