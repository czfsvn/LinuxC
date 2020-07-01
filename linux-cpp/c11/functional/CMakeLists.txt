cmake_minimum_required(VERSION 2.8)

project(function)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive -g")

include_directories(/usr/local/include)
set(LIBS boost_system)

#SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb -pg -fpermissive ")

add_executable(functional Functional.cpp)
add_executable(bind Bind.cpp)
add_executable(multi multi.cpp)

