cmake_minimum_required(VERSION 2.8)

project(gameutils)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -fpermissive -g")

include_directories(/usr/local/include)
set(LIBS pthread boost_system)

add_executable(main main.cpp Switches.cpp)