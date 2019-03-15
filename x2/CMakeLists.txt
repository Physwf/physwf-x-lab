cmake_minimum_required (VERSION 2.6)
project (x2)
aux_source_directory(. DIR_SRCS)
add_library(libx2 SHARED ${DIR_SRCS})