PROJECT(MikkTSpace)

CMAKE_MINIMUM_REQUIRED(VERSION 3.15)

INCLUDE_DIRECTORIES(inc)
ADD_LIBRARY(MikkTSpace STATIC src/mikktspace.c inc/mikktspace.h)
SET_TARGET_PROPERTIES(MikkTSpace PROPERTIES POSITION_INDEPENDENT_CODE TRUE)
SET(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

