cmake_minimum_required(VERSION 2.8)

project(function)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive -g")

include_directories(/usr/local/include)
set(LIBS boost_system)

#SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb -pg -fpermissive ")

add_executable(Main Main.cpp)
add_executable(gcc_o1 gcc_o1.cpp)
#add_executable(emplace emplace.cpp)

