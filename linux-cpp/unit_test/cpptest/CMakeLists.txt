cmake_minimum_required(VERSION 2.8)

include_directories(/usr/local/include/)
link_directories(/usr/local/lib/)

#add_definitions(-D_GLIBCXX_USE_CXX11_ABI=0 -std=c++11)
add_definitions(-std=c++11)

set(SRC Main.cpp)
add_executable(Main ${SRC})
target_link_libraries(Main cpptest)

set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -Wno-unused -Wno-write-strings -g2 -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")
