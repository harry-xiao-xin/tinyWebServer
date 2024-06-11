find_package(GTest REQUIRED)
aux_source_directory(../test DIR_TEST)
aux_source_directory(../test/test_log DIR_TEST_LOG)
aux_source_directory(../test/test_db DIR_TEST_DB)
aux_source_directory(../test/test_utils DIR_TEST_UTILS)
aux_source_directory(../test/test_timer DIR_TEST_TIMER)
set(link_lib glog gtest pthread)

aux_source_directory(../src/log DIR_LOG)
aux_source_directory(../src/db DIR_DB)
aux_source_directory(../src/timer DIR_TIMER)
aux_source_directory(../src/http DIR_HTTP)
aux_source_directory(../src/webserver DIR_WEBSERVER)
aux_source_directory(../src/config DIR_CONFIG)


add_executable(test_tinyWebServer ${DIR_TEST} ${DIR_TEST_LOG} ${DIR_TEST_DB} ${DIR_TEST_UTILS} ${DIR_TEST_TIMER} ${DIR_LOG} ${DIR_HTTP} ${DIR_WEBSERVER} ${DIR_DB} ${DIR_TIMER} ${DIR_CONFIG})
link_directories(/usr/local/lib/)
target_link_libraries(test_tinyWebServer ${link_lib} mysqlpp)