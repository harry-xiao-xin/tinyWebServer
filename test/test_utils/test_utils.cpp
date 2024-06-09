//
// Created by zpx on 2024/06/08.
//
#include <glog/logging.h>
#include <gtest/gtest.h>
#include "utils/block_queue.h"
#include <vector>
#include <thread>

#define  MAX_TEST_EPOCH 30
#define  BLOCK_QUEUE_SIZE 15
using namespace tiny_web_server;
TEST(test_block_queue, signle_thread_push_pop_function) {
    int tmp = -1;
    BlockQueue<int> block_queue(BLOCK_QUEUE_SIZE);
    for (int i = 0; i < MAX_TEST_EPOCH; i++) {
        if (!block_queue.push(i)) {
            LOG(INFO) << "block_queue push error.";
        }
        if (block_queue.pop(tmp) && tmp != i) {
            LOG(INFO) << "block_queue popping error.";
        }
    }
}

TEST(test_block_queue, signle_thread_size_function) {
    BlockQueue<int> block_queue(BLOCK_QUEUE_SIZE);
    for (int i = 0; i < BLOCK_QUEUE_SIZE; i++) {
        if (!block_queue.push(i)) {
            LOG(INFO) << "block_queue push error.";
        }
        ASSERT_EQ(block_queue.size(), (BLOCK_QUEUE_SIZE - i - 1) % BLOCK_QUEUE_SIZE);
    }
}

TEST(test_block_queue, single_thread_capacity_function) {
    BlockQueue<int> block_queue(BLOCK_QUEUE_SIZE);
    ASSERT_EQ(block_queue.capacity(), BLOCK_QUEUE_SIZE);
}

TEST(test_block_queue, single_thread_front_function) {
    BlockQueue<int> block_queue(BLOCK_QUEUE_SIZE);
    for (int i = 0; i < MAX_TEST_EPOCH; i++) {
        if (!block_queue.push(i)) {
            if (block_queue.full()) {
                LOG(INFO) << "block_queue full.";
                return;
            }
        } else {
            ASSERT_EQ(block_queue.front(), i);
        }
    }
}

TEST(test_block_queue, single_thread_tail_function) {
    int index = 0, tmp = -1;
    BlockQueue<int> block_queue(BLOCK_QUEUE_SIZE);
    for (int i = 0; i < MAX_TEST_EPOCH; i++) {
        if (i < BLOCK_QUEUE_SIZE) {
            block_queue.push(i);
        } else {
            ASSERT_EQ(block_queue.tail(), index);
            if (!block_queue.pop(tmp)) {
                LOG(INFO) << "block_queue pop error.";
            }
            ASSERT_EQ(tmp, index++);
        }
    }
}

TEST(test_block_queue, multi_thread_push_function) {
    std::vector<std::thread> threads(BLOCK_QUEUE_SIZE);
    BlockQueue<std::thread::id> block_queue(BLOCK_QUEUE_SIZE);
    for (auto &item: threads) {
        item = std::thread([] {});
        block_queue.push(item.get_id());
    }
    for (auto &item: threads) {
        item.join();
    }
    ASSERT_EQ(block_queue.size(), 0);
}
