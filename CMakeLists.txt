cmake_minimum_required (VERSION 3.4)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
project (x-lab)
add_subdirectory(x0)
include_directories(x0/Include)
add_subdirectory(x2)
include_directories(x2)
add_subdirectory(x3)
include_directories(x6/Include)
add_subdirectory(dx11demo)
add_subdirectory(JRenderer)
add_subdirectory(path_tracing)
add_subdirectory(ray_tracing)
add_subdirectory(pbrt)
add_subdirectory(MikkTSpace)
add_subdirectory(x8)
include_directories(x3/Include)
set(DIR_PROJECT "./demo")
add_executable(x-lab ${DIR_PROJECT}/winmain.cpp 
${DIR_PROJECT}/x2demo.cpp 
${DIR_PROJECT}/glut.h 
${DIR_PROJECT}/glut.cpp 
${DIR_PROJECT}/mesh.h 
${DIR_PROJECT}/mesh.cpp 
${DIR_PROJECT}/Cat.h 
${DIR_PROJECT}/Cat.cpp 
${DIR_PROJECT}/Lamborghini.h 
${DIR_PROJECT}/Lamborghini.cpp
${DIR_PROJECT}/material.h
${DIR_PROJECT}/material.cpp
${DIR_PROJECT}/light.h
${DIR_PROJECT}/light.cpp
${DIR_PROJECT}/color.h
${DIR_PROJECT}/color.cpp
${DIR_PROJECT}/texture.h
${DIR_PROJECT}/texture.cpp
${DIR_PROJECT}/vector.h
${DIR_PROJECT}/vector.cpp
${DIR_PROJECT}/scene.h
${DIR_PROJECT}/scene.cpp
)
target_link_libraries(x-lab libx2)
add_dependencies(x-lab libx2)

add_definitions(-DUNICODE -D_UNICODE)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS")

MESSAGE(STATUS "operation system is ${CMAKE_SYSTEM}")

IF (CMAKE_SYSTEM_NAME MATCHES "Linux")
	MESSAGE(STATUS "current platform: Linux ")
ELSEIF (CMAKE_SYSTEM_NAME MATCHES "Windows")
	MESSAGE(STATUS "current platform: Windows")
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
