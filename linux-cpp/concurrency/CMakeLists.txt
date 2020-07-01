cmake_minimum_required(VERSION 2.8)

project(concurrency)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -fpermissive -g")

include_directories(/usr/local/include)
set(LIBS pthread boost_system)

#SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb -pg -fpermissive ")

add_executable(thread_create thread_create.cpp)
add_executable(mutex mutex.cpp)
add_executable(condition condition.cpp)
add_executable(future future.cpp)
add_executable(future2 future2.cpp)
add_executable(atomic atomic.cpp)

add_executable(ThreadPool ThreadPool.cpp)

add_executable(chapter8 chapter8.cpp)
#add_executable(chapter9 Chapter9.cpp)

add_executable(behavior BeheviorLog.cpp)

target_link_libraries(mutex ${LIBS}) 
