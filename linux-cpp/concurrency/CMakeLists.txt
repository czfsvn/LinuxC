cmake_minimum_required(VERSION 2.8)

project(THREAD)

file(GLOB_RECURSE CPP_FILE *.cpp)

#set(SRC thread_create.cpp)
set(SRC mutex.cpp)

set(LIBS pthread)

add_definitions(-std=c++17)

add_executable(main ${SRC})

target_link_libraries(main ${LIBS})

SET(CMAKE_BUILD_TYPE "Debug")

SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb -pg -fpermissive ")

