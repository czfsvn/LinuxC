cmake_minimum_required(VERSION 2.8)
  
project(concurrency)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -fpermissive -g")

include_directories(/usr/local/include)
set(LIBS pthread boost_system)

#SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb -pg -fpermissive ")

add_subdirectory(Timer)
