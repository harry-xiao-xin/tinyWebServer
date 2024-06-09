find_package(GTest REQUIRED)
aux_source_directory(../test DIR_TEST)
aux_source_directory(../test/test_log DIR_TEST_LOG)
aux_source_directory(../test/test_threadpool DIR_TEST_THREADPOOL)
aux_source_directory(../test/test_db DIR_TEST_DB)
aux_source_directory(../test/test_utils DIR_TEST_UTILS)
set(link_lib glog gtest pthread)

aux_source_directory(../src/log DIR_LOG)
aux_source_directory(../src/threadpool DIR_THREADPOOL)
aux_source_directory(../src/db DIR_DB)


add_executable(test_tinyWebServer ${DIR_TEST} ${DIR_TEST_LOG} ${DIR_TEST_THREADPOOL} ${DIR_TEST_DB} ${DIR_TEST_UTILS} ${DIR_LOG} ${DIR_THREADPOOL} ${DIR_DB})
link_directories(/usr/local/lib/)
target_link_libraries(test_tinyWebServer ${link_lib} mysqlpp)