cmake_minimum_required(VERSION 2.8)

project(MAIN_PRO)

include_directories($ENV{CPPUNIT_HOME}/include)
link_directories($ENV{CPPUNIT_HOME}/lib)


add_definitions(-std=c++11)
add_definitions(-Wunused-variable)

file(GLOB SRC_FILES *.cpp)
#set(SRC_FILES Main.cpp)

add_executable(main ${SRC_FILES})
target_link_libraries(main cppunit dl)

SET(D_CXXFLAGS "-Wno-error=unused-but-set-variable")

message(status " D_CXXFLAGS: " ${D_CXXFLAGS})

SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} ${D_CXXFLAGS} -O0 -Wall -g2 -ggdb")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")
