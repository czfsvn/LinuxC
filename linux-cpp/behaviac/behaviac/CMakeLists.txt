cmake_minimum_required(VERSION 2.8)

#file(GLOB_RECURSE SRC_LIST ${PROJECT_SOURCE_DIR}/behaviac/src/*.*)
file(GLOB_RECURSE SRC_LIST ${PROJECT_SOURCE_DIR}/*.*)

add_library(behaviac ${SRC_LIST})
