﻿file(GLOB_RECURSE DIR_SRC
    "./*.cpp"
    "./*.h"
	"./*.rc"
	"./*.ico"
)
add_executable(JRenderer ${DIR_SRC})

MESSAGE(STATUS "FBX_SDK_HOME=$ENV{FBX_SDK_HOME}")
target_include_directories(dx11demo PRIVATE "../x6/Include")
target_include_directories(dx11demo PRIVATE $ENV{FBX_SDK_HOME}/include)
find_library(FBXSDK_LIBRARY libfbxsdk HINTS $ENV{FBX_SDK_HOME}/lib/vs2015/x86/release)

add_definitions(-DUNICODE -D_UNICODE)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS")

SET(EXECUTABLE_OUTPUT_PATH  ${CMAKE_BINARY_DIR})

MESSAGE(STATUS "operation system is ${CMAKE_SYSTEM}")

IF (CMAKE_SYSTEM_NAME MATCHES "Linux")
	MESSAGE(STATUS "current platform: Linux ")
ELSEIF (CMAKE_SYSTEM_NAME MATCHES "Windows")
	MESSAGE(STATUS "current platform: Windows")
	add_definitions(-DPLATFORM_WINDOWS)
	target_link_libraries (JRenderer "d3d12.lib")
	target_link_libraries (JRenderer "dxgi.lib")
	target_link_libraries (JRenderer "D3DCompiler.lib")
	target_link_libraries (JRenderer ${FBXSDK_LIBRARY})
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