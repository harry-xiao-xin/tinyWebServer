// 环形阻塞队列
// Created by zpx on 2024/06/08.
//

#ifndef TINYWEBSERVER_BLOCK_QUEUE_H
#define TINYWEBSERVER_BLOCK_QUEUE_H

#include <iostream>
#include <mutex>
#include <condition_variable>
#include <cassert>
#include <sys/time.h>

namespace tiny_web_server {
    template<typename T>
    class BlockQueue {
    public:
        /**
         * 创建阻塞队列
         * @param capacity 容量
         */
        explicit BlockQueue(int capacity);
        /**
         * 析构阻塞队列
         */
        ~BlockQueue();
        /**
         * 清空阻塞队列
         */
        void clear();
        /**
         * 队列是否满了
         * @return
         */
        bool full();
        /**
         * 队列是否为空
         * @return
         */
        bool empty();
        /**
         * 返回对头元素
         * @return
         */
        const T &front();
        /**
         * 返回队尾元素
         * @return
         */
        const T tail();
        /**
         * 队列大小
         * @return
         */
        const int size();
        /**
         * 队列容量
         * @return
         */
        const int capacity();
        /**
         * 添加元素
         * @param item
         * @return 成功返回true，失败返回false
         */
        bool push(const T &item);
        /**
         * 弹出队尾元素
         * @return
         */
        bool pop(T &item);
        /**
         * 定时弹出队尾元素
         * @param item
         * @param ms_timeout
         * @return
         */
        bool pop(T &item, int ms_timeout);
    private:
        std::mutex mutex_;
        std::condition_variable condition_variable_;
        T *data_;
        int front_;
        int tail_;
        int size_;
        int capacity_;
    };

    template<typename T>
    BlockQueue<T>::BlockQueue(int capacity) {
        assert(capacity >= 0);
        this->capacity_ = capacity;
        data_ = new T[capacity];
        size_ = capacity;
        front_ = 0;
        tail_ = 0;
    }

    template<typename T>
    BlockQueue<T>::~BlockQueue() {
        std::lock_guard<std::mutex> lock(mutex_);
        delete[]data_;
        data_ = nullptr;
    }

    template<typename T>
    void BlockQueue<T>::clear() {
        std::lock_guard<std::mutex> lock(mutex_);
        front_ = 0;
        tail_ = 0;
        size_ = capacity_;
    }

    template<typename T>
    bool BlockQueue<T>::full() {
        std::lock_guard<std::mutex> lock(mutex_);
        return size_ == 0;
    }

    template<typename T>
    bool BlockQueue<T>::empty() {
        return size_ == capacity_;
    }

    template<typename T>
    const T &BlockQueue<T>::front() {
        assert(!empty());
        std::lock_guard<std::mutex> lock(mutex_);
        return data_[(front_ - 1) % capacity_];
    }

    template<typename T>
    const T BlockQueue<T>::tail() {
        assert(!empty());
        std::lock_guard<std::mutex> lock(mutex_);
        return data_[tail_ % capacity_];
    }

    template<typename T>
    const int BlockQueue<T>::size() {
        std::lock_guard<std::mutex> lock(mutex_);
        return size_;
    }

    template<typename T>
    const int BlockQueue<T>::capacity() {
        std::lock_guard<std::mutex> lock(mutex_);
        return capacity_;
    }

    template<typename T>
    bool BlockQueue<T>::push(const T &item) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (size_ == 0) return false;
        data_[front_ % capacity_] = item;
        front_ = front_ + 1;
        size_ = capacity_ - (front_ - tail_);
        return true;
    }

    template<typename T>
    bool BlockQueue<T>::pop(T &item) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (size_ == capacity_)return false;
        item = data_[tail_ % capacity_];
        tail_ = tail_ + 1;
        size_ = capacity_ - (front_ - tail_);
        return true;
    }

    template<typename T>
    bool BlockQueue<T>::pop(T &item, int ms_timeout) {
        std::unique_lock<std::mutex> lock(mutex_);
        if (size_ <= 0) {
            if (condition_variable_.wait_for(lock, std::chrono::milliseconds(ms_timeout)) != std::cv_status::timeout) {
                lock.unlock();
                return false;
            }
        }
        if (size_ <= 0) {
            lock.unlock();
            return false;
        }
        tail_ = (tail_ + 1) % capacity_;
        item = data_[tail_];
        size_ = capacity_ - (front_ - tail_);
        lock.unlock();
        return true;
    }
}
#endif //TINYWEBSERVER_BLOCK_QUEUE_H
