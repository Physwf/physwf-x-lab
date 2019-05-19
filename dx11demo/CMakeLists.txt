set(DIR_PROJECT ".")

file(COPY ${DIR_PROJECT}/Triangle.vs DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${DIR_PROJECT}/Triangle.ps DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
configure_file(${DIR_PROJECT}/Triangle.vs ${CMAKE_CURRENT_BINARY_DIR}/Triangle.vs COPYONLY)
configure_file(${DIR_PROJECT}/Triangle.ps ${CMAKE_CURRENT_BINARY_DIR}/Triangle.ps COPYONLY)
add_executable(dx11demo 
${DIR_PROJECT}/main.cpp
${DIR_PROJECT}/EnumAdapter.cpp
${DIR_PROJECT}/D3D11Triangle.cpp
${DIR_PROJECT}/Triangle.vs
${DIR_PROJECT}/Triangle.ps
)
add_definitions(-DUNICODE -D_UNICODE)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS")


MESSAGE(STATUS "operation system is ${CMAKE_SYSTEM}")

IF (CMAKE_SYSTEM_NAME MATCHES "Linux")
	MESSAGE(STATUS "current platform: Linux ")
ELSEIF (CMAKE_SYSTEM_NAME MATCHES "Windows")
	MESSAGE(STATUS "current platform: Windows")
	target_link_libraries (dx11demo "d3d11.lib")
	target_link_libraries (dx11demo "dxgi.lib")
	target_link_libraries (dx11demo "D3DCompiler.lib")
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
