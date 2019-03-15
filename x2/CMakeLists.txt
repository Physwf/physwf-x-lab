cmake_minimum_required (VERSION 2.6)
project (x1)
aux_source_directory(. DIR_SRCS)
add_library(libx1 SHARED ${DIR_SRCS})