cmake_minimum_required(VERSION 2.8)

project(designmodel)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -fpermissive -g")

include_directories(/usr/local/include)
set(LIBS pthread boost_system)

add_executable(adapter adapter/1.cpp)
add_executable(builder builder/1.cpp)
#add_executable(command command/1.cpp)
add_executable(composite composite/1.cpp)
#add_executable(factory1 factory/1.cpp)
#add_executable(factory2 factory/2.cpp)
#add_executable(factory3 factory/3.cpp)
add_executable(factory4 factory/4.cpp)
#add_executable(prototype adapter/1.cpp)
add_executable(iterator iterator/1.cpp)
add_executable(strategy strategy/1.cpp)