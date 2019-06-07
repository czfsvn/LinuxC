cmake_minimum_required(VERSION 2.8)

#message(STATUS "dir: ${CMAKE_CURRENT_SOURCE_DIR}")
#message(STATUS "LIBS: ${LIBS}")

file(GLOB SRC_LIST "${CMAKE_CURRENT_SOURCE_DIR}/behaviac_generated/types/internal/*.cpp")
file(GLOB SRC *.cpp)

add_executable(tutorial_1 ${SRC} ${SRC_LIST})

target_link_libraries(tutorial_1 ${LIBS})
