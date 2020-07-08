cmake_minimum_required(VERSION 2.8)

project(template)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive -g")

include_directories(/usr/local/include)
set(LIBS boost_system)

#SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O3 -Wall -g -ggdb -pg -fpermissive ")

add_executable(main 1.cpp)
add_executable(chapter2 chapter2.cpp)
add_executable(chapter3 chapter3.cpp)

