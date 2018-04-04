cmake_minimum_required(VERSION 2.8)
include_directories($ENV{PROTOBUF_HOME}/include)
link_directories($ENV{PROTOBUF_HOME}/lib)

project(MAIN_PRO)

set(PROTOC "protoc")

add_definitions(-std=c++11)

file(GLOB_RECURSE PROTO_FILE *.proto)

foreach(pb ${PROTO_FILE})
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}
        COMMAND ${PROTOC} --cpp_out=${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/${pb}
        )
endforeach(pb ${PROTO_FILE})

set(SRC Main.cpp person.pb.cc)

add_executable(main ${SRC})
target_link_libraries(main protobuf)

SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")
