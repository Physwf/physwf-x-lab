﻿set(DIR_PROJECT "${CMAKE_CURRENT_SOURCE_DIR}/")
file(GLOB_RECURSE DIR_SRC
    "./*.cpp"
    "./*.h"
)
file(GLOB_RECURSE SHADER_SRC
    "./*.hlsl"
)
foreach(_source IN ITEMS ${DIR_SRC})
    get_filename_component(_source_path "${_source}" PATH)
	file(RELATIVE_PATH _source_path_rel "${DIR_PROJECT}" "${_source_path}")
    string(REPLACE "/" "\\" _group_path "${_source_path_rel}")
    source_group("${_group_path}" FILES "${_source}")
endforeach()

add_executable(dx11demo ${DIR_SRC})
#add_executable(dx11demo ${DIR_SRC})
set(DirectXTexDir "D:/DirectXTex/")
MESSAGE(STATUS "FBX_SDK_HOME=$ENV{FBX_SDK_HOME}")
MESSAGE(STATUS "FBX_SDK_HOME=$ENV{FBX_SDK_HOME}")
target_include_directories(dx11demo PRIVATE "")
target_include_directories(dx11demo PRIVATE "CompositionLighting")
target_include_directories(dx11demo PRIVATE "Material")
target_include_directories(dx11demo PRIVATE "Math")
target_include_directories(dx11demo PRIVATE "Mesh")
target_include_directories(dx11demo PRIVATE "PostProcess")
target_include_directories(dx11demo PRIVATE "Renderer")
target_include_directories(dx11demo PRIVATE "RHI")
target_include_directories(dx11demo PRIVATE "Scene")
target_include_directories(dx11demo PRIVATE "Utilities")
target_include_directories(dx11demo PRIVATE "../MikkTSpace/inc")
target_include_directories(dx11demo PRIVATE "../MikkTSpace/inc")
target_include_directories(dx11demo PRIVATE $ENV{FBX_SDK_HOME}/include)
find_library(FBXSDK_LIBRARY libfbxsdk HINTS $ENV{FBX_SDK_HOME}/lib/vs2015/x86/release)
target_include_directories(dx11demo PRIVATE ${DirectXTexDir}DirectXTex)
find_library(DirectXTexLib DirectXTex HINTS ${DirectXTexDir}DirectXTex/Bin/Desktop_2017_Win10/Win32/Debug/)

#https://www.jianshu.com/p/66df9650a9e2
ADD_CUSTOM_COMMAND(TARGET dx11demo
        PRE_BUILD
        COMMENT "Copy Shaders"
        COMMAND @copy $(SolutionDir)..\\dx11demo\\Shaders\\*.hlsl  $(TargetDir)
)

add_definitions(-DUNICODE -D_UNICODE)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS")

#configure_file(${DIR_PROJECT}/Triangle.hlsl ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)
#copy $(SolutionDir)..\dx11demo\Shaders\*.hlsl  $(TargetDir)
SET(EXECUTABLE_OUTPUT_PATH  ${CMAKE_BINARY_DIR})

MESSAGE(STATUS "operation system is ${CMAKE_SYSTEM}")

IF (CMAKE_SYSTEM_NAME MATCHES "Linux")
	MESSAGE(STATUS "current platform: Linux ")
ELSEIF (CMAKE_SYSTEM_NAME MATCHES "Windows")
	MESSAGE(STATUS "current platform: Windows")
	add_definitions(-DPLATFORM_WINDOWS)
	target_link_libraries(dx11demo MikkTSpace)
	target_link_libraries (dx11demo "d3d11.lib")
	target_link_libraries (dx11demo "d3d9.lib")
	target_link_libraries (dx11demo "dxgi.lib")
	target_link_libraries (dx11demo "D3DCompiler.lib")
	target_link_libraries (dx11demo ${FBXSDK_LIBRARY})
	target_link_libraries (dx11demo ${DirectXTexLib})
	target_link_libraries (dx11demo dxguid.lib)
ELSEIF (CMAKE_SYSTEM_NAME MATCHES "FreeBSD")
	MESSAGE(STATUS "current platform: FreeBSD")
ELSE ()
	MESSAGE(STATUS "other platform: ${CMAKE_SYSTEM_NAME}")
ENDIF (CMAKE_SYSTEM_NAME MATCHES "Linux")

MESSAGE(STATUS "###################################")

IF (WIN32)
	MESSAGE(STATUS "Now is windows")
ELSEIF (APPLE)
	MESSAGE(STATUS "Now is Apple systens.")
ELSEIF (UNIX)
	MESSAGE(STATUS "Now is UNIX-like OS's. Including aPPLE os x  and CygWin")
ENDIF ()
