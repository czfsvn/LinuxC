cmake_minimum_required(VERSION 2.8)
include_directories(/usr/local/Cellar/protobuf/3.5.0.1/include)
link_directories(/usr/local/Cellar/protobuf/3.5.0.1/lib)

project(MAIN_PRO)

add_definitions(-std=c++11)

set(SRC Main.cpp person.pb.cc)

add_executable(main ${SRC})
target_link_libraries(main protobuf)

SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")
