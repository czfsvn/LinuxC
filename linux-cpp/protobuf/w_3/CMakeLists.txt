cmake_minimum_required(VERSION 2.8)
include_directories($ENV{PROTOBUF_HOME}/include)
link_directories($ENV{PROTOBUF_HOME}/lib)

project(MAIN_PRO)

set(PROTOC "protoc")

set(PROTOS_DIR          ${CMAKE_CURRENT_SOURCE_DIR})
set(PROTOS_CPPOUT_DIR   ${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB_RECURSE PROTO_FILE *.proto)
file(GLOB_RECURSE CPP_FILE *.cpp)

foreach(PB ${PROTO_FILE})
    get_filename_component(PB_NAME ${PB} NAME_WE)
    get_filename_component(PB_PATH ${PB} PATH)

    set(PB_H        ${PB_NAME}.pb.h)
    set(PB_CPP      ${PB_NAME}.pb.cc)

    set(PB_CPPOUT   ${PROTOS_CPPOUT_DIR}/${PB_H}    ${PROTOS_CPPOUT_DIR}/${PB_CPP})

    set(PB_CPPOUTS  ${PB_CPPOUTS}   ${PB_CPPOUT})

    add_custom_command(OUTPUT ${PB_CPPOUT}
        COMMAND ${PROTOC} -I=${PROTOS_DIR} --cpp_out=${PROTOS_DIR} --proto_path=${PB_PATH} ${PB}
        DEPENDS ${PB})

endforeach(PB ${PROTO_FILE})

add_definitions(-std=c++11)
set(SRC Main.cpp ${PB_CPPOUTS})

add_executable(main ${SRC})
target_link_libraries(main protobuf rt)

SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb -pg -fpermissive")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

