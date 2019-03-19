set_property(GLOBAL PROPERTY USE_FOLDERS ON)
cmake_minimum_required (VERSION 2.6)
project (x3)

aux_source_directory(./Include DIR_Inc)
aux_source_directory(./Source DIR_SRCS)
aux_source_directory(./Source/BSDSockets DIR_SRCS)
aux_source_directory(./Source/Windows DIR_SRCS)


if (CMAKE_SYSTEM_NAME MATCHES "Linux")
	message(status " current platform: Linux ")
elseif(CMAKE_SYSTEM_NAME MATCHES "Windows")
	message(status " current platform: Windows")
	add_definitions(-DPLATFORM_WINDOWS)
elseif(CMAKE_SYSTEM_NAME MATCHES "FreeBSD")
	message(status " current platform: FreeBSD")
else()
	message(status "other platform: ${CMAKE_SYSTEM_NAME}")
endif()

add_library(libx3 SHARED ${DIR_Inc} ${DIR_SRCS})
set_target_properties(libx3 PROPERTIES FOLDER "Libraries")

