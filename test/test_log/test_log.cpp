//
// Created by zpx on 2024/06/08.
//
#include <gtest/gtest.h>
#include <glog/logging.h>
#include "log/log.h"

using namespace tiny_web_server;
TEST(test_log, test_single_thread) {
    Log::get_instance()->init("./test_log", false, 2000, 800000, 0);
    LOG_INFO("%s:", "test info log");
    LOG_ERROR("%s:", "test error log");
    LOG_DEBUG("%s:", "test debug log");
    LOG_WARN("%s:", "test warn log");
}

TEST(test_log, test_multi_thread) {
    Log::get_instance()->init("./test_log", true, 200, 4, 1);
    LOG_INFO("%s:", "test info log");
    LOG_ERROR("%s:", "test error log");
    LOG_DEBUG("%s:", "test debug log");
    LOG_WARN("%s:", "test warn log");
}



