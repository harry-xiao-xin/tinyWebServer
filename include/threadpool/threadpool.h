//
// Created by zpx on 2024/06/08.
//

#ifndef TINYWEBSERVER_THREADPOOL_H
#define TINYWEBSERVER_THREADPOOL_H

#include <list>
#include <cstdio>
#include <exception>
#include <thread>
#include <mutex>
#include "db/sql_connection_pool.h"
#include <vector>
#include "http/http.h"

namespace tiny_web_server {
    template<typename T>
    class ThreadPool {
    public:
        ThreadPool(int actor_model, ConnectionPool *connPool, int thread_number = 8, int max_requests = 10000);
        ~ThreadPool();
        bool append(T *request, int state);
        bool append_p(T *request);
    private:
        /**
         * 工作线程
         */
        static void worker(void *arg);
        void run();
    private:
        int m_thread_number_; // 线程数
        int m_max_requests_; // 允许的最大请求数
        std::vector<std::thread> m_threads_; // 线程池数组
        std::list<T *> m_work_queue_; // 请求队列
        std::mutex m_mutex_;
        std::condition_variable m_conditionVariable_;
        ConnectionPool *m_connection_pools_;
        int m_actor_model_;
    };

    template<typename T>
    ThreadPool<T>::ThreadPool(int actor_model, ConnectionPool *connPool, int thread_number, int max_requests):
            m_actor_model_(actor_model), m_thread_number_(thread_number), m_max_requests_(max_requests), m_connection_pools_(connPool) {
        if (thread_number <= 0 || max_requests <= 0)throw std::runtime_error("thread number or max_request less than 1");
        m_threads_.reserve(thread_number);
        for (int i = 0; i < thread_number; ++i) {
            m_threads_.push_back(std::thread(worker, this));
            m_threads_[i].detach();
        }
    }

    template<typename T>
    ThreadPool<T>::~ThreadPool() {
        std::vector<std::thread>().swap(m_threads_);
    }

    template<typename T>
    bool ThreadPool<T>::append(T *request, int state) {
        std::scoped_lock<std::mutex> lock(m_mutex_);
        if (m_work_queue_.size() >= m_max_requests_) {
            return false;
        }
        request->m_state = state;
        m_work_queue_.push_back(request);
        m_conditionVariable_.notify_one();
        return true;
    }

    template<typename T>
    bool ThreadPool<T>::append_p(T *request) {
        std::scoped_lock<std::mutex> lock(m_mutex_);
        if (m_work_queue_.size() >= m_max_requests_) {
            return false;
        }
        m_work_queue_.push_back(request);
        m_conditionVariable_.notify_one();
        return true;
    }

    template<typename T>
    void ThreadPool<T>::worker(void *arg) {
        ThreadPool *pool = (ThreadPool *) arg;
        pool->run();
    }

    template<typename T>
    void ThreadPool<T>::run() {
        while (true) {
            std::unique_lock<std::mutex> lock(m_mutex_);
            m_conditionVariable_.wait(lock, [&] { return m_work_queue_.size() > 0; });
            T *request = m_work_queue_.front();
            m_work_queue_.pop_front();
            lock.unlock();
            if (!request)continue;
            if (1 == m_actor_model_) {
                if (0 == request->m_state) {
                    if (request->read_once()) {
                        request->improv = 1;
                        request->mysql = m_connection_pools_->getConnection();
                        request->process();
                    } else {
                        request->improv = 1;
                        request->timer_flag = 1;
                    }
                } else {
                    if (request->write()) {
                        request->improv = 1;
                    } else {
                        request->improv = 1;
                        request->timer_flag = 1;
                    }
                }
            } else {
                request->mysql = m_connection_pools_->getConnection();
                request->process();
            }
        }
    }
}
#endif //TINYWEBSERVER_THREADPOOL_H
