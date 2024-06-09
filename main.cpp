#include <iostream>
#include "log/log.h"

using namespace tiny_web_server;

int main() {
    Log::get_instance()->init("./test_log", false, 2000, 800000, 0);
    LOG_INFO("%s:", "test info log");
    LOG_ERROR("%s:", "test error log");
    LOG_DEBUG("%s:", "test debug log");
    LOG_WARN("%s:", "test warn log");
    std::cout << "Hello, World!" << std::endl;
    return 0;
}
