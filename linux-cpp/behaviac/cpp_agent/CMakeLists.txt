cmake_minimum_required(VERSION 2.8)

file(GLOB SRC_LIST "${CMAKE_CURRENT_SOURCE_DIR}/workspace/exported/behaviac_generated/types/internal/*.cpp")
file(GLOB SRC *.cpp)

add_executable(cpp_agent ${SRC} ${SRC_LIST})

target_link_libraries(cpp_agent ${LIBS})
