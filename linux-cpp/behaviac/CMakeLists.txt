cmake_minimum_required(VERSION 2.8)

#message(STATUS "inc_dirs : ${PROJECT_SOURCE_DIR}/behaviac/inc")

project(behaviac_proj)

set(CMAKE_CXX_COMPILER "/usr/bin/g++")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -std=gnu++11")

#set(LIBRARY_OUT_DIR ${CMAKE_BINARY_DIR}/lib)
#set(RUNTIME_OUT_DIR ${CMAKE_BINARY_DIR}/bin)

set(LIBRARY_OUTPUT_DIR ${PROJECT_SOURCE_DIR}/lib)
set(BINARY_OUTPUT_DIR ${PROJECT_SOURCE_DIR}/bin)

#message(STATUS "lib: ${LIBRARY_OUTPUT_DIR}")
#message(STATUS "bin: ${BINARY_OUTPUT_DIR}")

include_directories(${PROJECT_SOURCE_DIR}/behaviac/inc)
link_directories(${LIBRARY_OUT_DIR})

set(LIBS behaviac pthread)

add_subdirectory("${PROJECT_SOURCE_DIR}/behaviac")
add_subdirectory("${PROJECT_SOURCE_DIR}/tutorials/tutorial_1/cpp")

