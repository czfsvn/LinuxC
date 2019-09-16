cmake_minimum_required(VERSION 2.8)

project(MAIN_PRO)

file(GLOB_RECURSE CPP_FILE *.cpp)

set(SRC Main.cpp Card.cpp)

add_definitions(-std=c++11)

add_executable(main ${SRC})

SET(CMAKE_BUILD_TYPE "Debug")

SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb -pg -fpermissive")



