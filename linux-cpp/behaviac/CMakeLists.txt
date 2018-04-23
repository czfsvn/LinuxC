cmake_minimum_required(VERSION 2.8)

project(behaviac_proj)

set(CMAKE_C_COMPILER "/usr/bin/gcc")
set(CMAKE_CXX_COMPILER "/usr/bin/g++")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -std=gnu++11")

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

set(LIBS pthread behaviac)

include_directories(${PROJECT_SOURCE_DIR}/behaviac/inc)
link_directories(${LIBRARY_OUTPUT_PATH})

#message(STATUS "PROJECT_SOURCE_DIR : ${PROJECT_SOURCE_DIR}")
#message(STATUS "CMAKE_SOURCE_DIR : ${CMAKE_SOURCE_DIR}")

add_subdirectory("${PROJECT_SOURCE_DIR}/behaviac")
add_subdirectory("${PROJECT_SOURCE_DIR}/tutorials/tutorial_1/cpp")
add_subdirectory("${PROJECT_SOURCE_DIR}/cpp_agent")

