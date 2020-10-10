include_directories(/usr/include/mysql/)
link_directories(/usr/lib64/mysql)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/libs)
set(LIB_SRC MysqlCon.cpp Mysqlres.cpp)
add_library(mysql_cn ${LIB_SRC})
target_link_libraries(mysql_cn mysqlclient_r)


SET(CMAKE_BUILD_TYPE "Debug")  
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")  
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall") 

