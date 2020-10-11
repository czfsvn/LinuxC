cmake_minimum_required(VERSION 2.8)

project(stl)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive -g")

include_directories(/usr/local/include)
set(LIBS boost_system)

#SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb -pg -fpermissive ")

add_subdirectory("${PROJECT_SOURCE_DIR}/sort")
add_subdirectory("${PROJECT_SOURCE_DIR}/Container")
add_subdirectory("${PROJECT_SOURCE_DIR}/Construction")
add_subdirectory("${PROJECT_SOURCE_DIR}/attribute")
