//
// Created by zpx on 2024/06/08.
//
#include <gtest/gtest.h>
#include <glog/logging.h>

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    google::InitGoogleLogging("TinyWebServer");
    FLAGS_log_dir = "./log";
    FLAGS_alsologtostderr = true;
    LOG(INFO) << "TinyWebServer start testing...\n";
    return RUN_ALL_TESTS();
}