//
// Created by zpx on 2024/06/09.
//
#include "timer/timer.h"

namespace tiny_web_server {
    void Utils::init(int time_slot) {
        m_TIMESLOT = time_slot;
    }

    int Utils::setNonBlocking(int fd) {
        int old_option = fcntl(fd, F_GETFL);
        int new_option = old_option | O_NONBLOCK;
        fcntl(fd, F_SETFL, new_option);
        return old_option;
    }

    void Utils::addFd(int epoll_fd, int fd, bool one_shot, int trigMode) {
        epoll_event event;
        event.data.fd = fd;
        if (1 == trigMode) {
            event.events = EPOLLIN | EPOLLET | EPOLLRDHUP;
        } else {
            event.events = EPOLLIN | EPOLLRDHUP;
        }
        if (one_shot)
            event.events |= EPOLLONESHOT;
        epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event);
        setNonBlocking(fd);
    }

    void Utils::sigHandler(int sig) {
        int save_errno = errno;
        int msg = sig;
        send(u_pipe_fd[1], (char *) &msg, 1, 0);
        errno = save_errno;
    }

    void Utils::addSig(int sig, void (*handler)(int), bool restart) {
        struct sigaction sa;
        bzero(&sa, sizeof(sa));
        sa.sa_handler = handler;
        if (restart)
            sa.sa_flags |= SA_RESTART;
        sigfillset(&sa.sa_mask);
        assert(sigaction(sig, &sa, nullptr) != -1);
    }

    void Utils::tick() {
        time_t cur = time(nullptr);
        auto base = &sortTimerList.top(); // root of the binary heap
        while (!sortTimerList.empty()) {
            if (base == nullptr || cur < (*base)->expire)break;
            (*base)->cb_func((*base)->user_data);
            sortTimerList.pop();
        }
    }

    void Utils::timerHandler() {
        tick();
        alarm(m_TIMESLOT);
    }

    void Utils::showError(int conn_fd, const char *info) {
        send(conn_fd, info, strlen(info), 0);
        close(conn_fd);
    }

    int *Utils::u_pipe_fd = nullptr;
    int Utils::u_epoll_fd = 0;

    class Utils;

    void cb_func(ClientData *user_data) {
        epoll_ctl(Utils::u_epoll_fd, EPOLL_CTL_DEL, user_data->sock_fd, 0);
        assert(user_data);
        close(user_data->sock_fd);
        http_conn::m_user_count--;
    }
}
