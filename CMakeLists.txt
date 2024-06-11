cmake_minimum_required(VERSION 3.15)
project(tinyWebServer)
set(CMAKE_CXX_STANDARD 17)

# local lib
include_directories(./include)
set(link_lib glog pthread gtest)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)


# mysql
include_directories(/usr/include/mysql)
include_directories(/usr/local/include/mysql++)

aux_source_directory(./src/log LIB_LOG)
aux_source_directory(./src/db LIB_DB)
aux_source_directory(./src/timer LIB_TIMER)
aux_source_directory(./src/http LIB_HTTP)
aux_source_directory(./src/webserver LIB_WEBSERVER)
aux_source_directory(./src/config LIB_CONFIG)

add_executable(tinyWebServer main.cpp ${LIB_LOG} ${LIB_THREADPOOL} ${LIB_DB} ${LIB_TIMER} ${LIB_HTTP} ${LIB_WEBSERVER} ${LIB_CONFIG})
target_include_directories(${PROJECT_NAME} PUBLIC ${GTEST_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${link_lib})
target_link_libraries(${PROJECT_NAME} mysqlpp)
enable_testing()
add_subdirectory(test)