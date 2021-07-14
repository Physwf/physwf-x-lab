cmake_minimum_required (VERSION 3.15)
file(GLOB_RECURSE DIR_SRC
    "./*.cpp"
    "./*.h"
	"./*.rc"
	"./*.ico"
)
add_executable(JRenderer ${DIR_SRC})

set(DirectXTexDir "E:/DirectXTex/")
set(DirectXMathDir "E:/DirectXMath/")

MESSAGE(STATUS "FBX_SDK_HOME=$ENV{FBX_SDK_HOME}")
target_include_directories(JRenderer PRIVATE "../x0/Include")
target_include_directories(JRenderer PRIVATE $ENV{FBX_SDK_HOME}/include)
find_library(FBXSDK_LIBRARY libfbxsdk PATHS $ENV{FBX_SDK_HOME}\\lib\\vs2015\\x64\\debug)
find_library(DirectXTexLib DirectXTex HINTS ${DirectXTexDir}DirectXTex/Bin/Desktop_2017_Win10/x64/Debug/)
target_include_directories(JRenderer PRIVATE ${DirectXTexDir}DirectXTex)
target_include_directories(JRenderer PRIVATE ${DirectXMathDir}Inc)

#https://www.jianshu.com/p/66df9650a9e2
ADD_CUSTOM_COMMAND(TARGET JRenderer
        PRE_BUILD
        COMMENT "Copy Shaders"
        COMMAND @copy $(SolutionDir)..\\JRenderer\\Shaders\\*.hlsl  $(TargetDir)
)

add_definitions(-DUNICODE -D_UNICODE)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS")

SET(EXECUTABLE_OUTPUT_PATH  ${CMAKE_BINARY_DIR})

set_property(TARGET JRenderer PROPERTY VS_PACKAGE_REFERENCES "WinPixEventRuntime_1.0.*")

MESSAGE(STATUS "operation system is ${CMAKE_SYSTEM}")
MESSAGE(STATUS "FBXSDK_LIBRARY  is ${FBXSDK_LIBRARY}")

IF (CMAKE_SYSTEM_NAME MATCHES "Linux")
	MESSAGE(STATUS "current platform: Linux ")
ELSEIF (CMAKE_SYSTEM_NAME MATCHES "Windows")
	MESSAGE(STATUS "current platform: Windows")
	add_definitions(-DPLATFORM_WINDOWS)
	target_link_libraries (JRenderer "d3d12.lib")
	target_link_libraries (JRenderer "dxgi.lib")
	target_link_libraries (JRenderer "dxguid.lib")
	target_link_libraries (JRenderer "D3DCompiler.lib")
	target_link_libraries (JRenderer ${FBXSDK_LIBRARY})
	target_link_libraries (JRenderer ${DirectXTexLib})
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
