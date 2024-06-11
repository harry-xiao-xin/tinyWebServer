//
// Created by zpx on 2024/06/09.
//

#ifndef TINYWEBSERVER_TIMER_H
#define TINYWEBSERVER_TIMER_H

#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cassert>
#include <cstring>
#include <thread>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <stdarg.h>
#include <errno.h>
#include <sys/wait.h>
#include "log/log.h"
#include "utils/wrap_priority_queue.h"
#include "http/http.h"

namespace tiny_web_server {
    class Timer;

    struct ClientData {
        sockaddr_in address;
        int sock_fd;
        Timer *timer;
    };

    class Timer {
    public:
        Timer() {}

        time_t expire;
        void (*cb_func)(ClientData *);
        ClientData *user_data;
    };

    class Utils {
    private:
        struct SortByExpire {
            bool operator()(const Timer *t1, const Timer *t2) {
                return t1->expire < t2->expire;
            }
        };

        void tick();
    public:
        Utils() = default;
        ~Utils() = default;
        void init(int time_slot);
        static int setNonBlocking(int fd);
        static void addFd(int epoll_fd, int fd, bool one_shot, int trigMode);
        static void sigHandler(int sig);
        void addSig(int sig, void(handler)(int), bool restart = true);
        void timerHandler();
        void showError(int conn_fd, const char *info);
    public:
        static int *u_pipe_fd;
        static int u_epoll_fd;
        WrapPriorityQueue<Timer *, std::vector<Timer *>, SortByExpire> sortTimerList;       // 升序队列
        int m_TIMESLOT;
    public:
    };

    void cb_func(ClientData *clientData);
}
#endif //TINYWEBSERVER_TIMER_H
