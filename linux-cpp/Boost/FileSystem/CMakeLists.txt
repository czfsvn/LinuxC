cmake_minimum_required(VERSION 2.8)

project(function)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive -g -O0")
set(LIBS boost_system boost_filesystem crypto boost_iostreams)

include_directories(/usr/local/include /usr/local/boost_1_75/include)
link_directories(${LIBS} /usr/local/lib /usr/local/boost_1_75/lib)

#SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb -pg -fpermissive")

add_executable(main Main.cpp)
target_link_libraries(main ${LIBS})

add_executable(filewatcher FileWatcher/Main.cpp FileWatcher/FileManager.cpp)
target_link_libraries(filewatcher ${LIBS})
