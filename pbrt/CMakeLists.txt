cmake_minimum_required (VERSION 3.4)

set(DIR_PROJECT ".")

add_executable(pbrt 
${DIR_PROJECT}/main.cpp
${DIR_PROJECT}/pbrt.h
${DIR_PROJECT}/geometry.h
${DIR_PROJECT}/spectrum.h
${DIR_PROJECT}/spectrum.cpp
${DIR_PROJECT}/reflection.h
${DIR_PROJECT}/reflection.cpp
)

add_definitions(-DUNICODE -D_UNICODE)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS")

MESSAGE(STATUS "operation system is ${CMAKE_SYSTEM}")

IF (CMAKE_SYSTEM_NAME MATCHES "Linux")
	MESSAGE(STATUS "current platform: Linux ")
ELSEIF (CMAKE_SYSTEM_NAME MATCHES "Windows")
	MESSAGE(STATUS "current platform: Windows")
	target_link_libraries (pbrt "gdiplus.lib")
	target_link_libraries (pbrt "Shlwapi.lib")
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
