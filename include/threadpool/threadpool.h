////
//// Created by zpx on 2024/06/08.
////
//
//#ifndef TINYWEBSERVER_THREADPOOL_H
//#define TINYWEBSERVER_THREADPOOL_H
//
//#include <list>
//#include <cstdio>
//#include <exception>
//#include <thread>
//#include <mutex>
//#include "db/sql_connection_pool.h"
//
//namespace tiny_web_server {
//    template<typename T>
//    class ThreadPool {
//    public:
//        ThreadPool(int actor_model, ConnectionPool *connPool, int thread_number = 8, int max_request = 10000);
//        ~ThreadPool();
//        bool append(T *request, int state);
//        bool append_p(T *request);
//    private:
//        /**
//         * 工作线程
//         */
//        static void worker();
//        void run();
//    };
//}
//#endif //TINYWEBSERVER_THREADPOOL_H
