cmake_minimum_required(VERSION 2.8)

project(function)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive -g")

include_directories(/usr/local/include)
set(LIBS boost_system)

#SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb -pg -fpermissive ")

add_executable(file_notify file_notify.cpp)
add_executable(daemon daemon.cpp)
add_executable(defer defer.cpp)
add_executable(time time.cpp)

