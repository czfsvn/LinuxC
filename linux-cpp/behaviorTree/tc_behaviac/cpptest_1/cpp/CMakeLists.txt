cmake_minimum_required (VERSION 2.8)

message(STATUS "tutorials: ${CMAKE_CURRENT_SOURCE_DIR}") 

#SET(CMAKE_C_FLAGS_DEBUG "-g -Wall -std=gnu++11")
#SET(CMAKE_CXX_FLAGS_DEBUG "-g -Wall -std=gnu++11")
#SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -std=c++11")

include_directories(/home/czf/work/git/LinuxC/linux-cpp/behaviorTree/tc_behaviac/inc/)
link_directories(/home/czf/work/git/LinuxC/linux-cpp/behaviorTree/tc_behaviac/lib)

set(SRC 
    Main.cpp 
    Player.cpp
    behaviac_generated/types/internal/PlayerAgent.cpp
    behaviac_generated/types/internal/behaviac_agent_meta.cpp
    )

add_executable(cpp_test_1 ${SRC})

target_link_libraries(cpp_test_1 pthread behaviac_gcc_release)
