//
// Created by zpx on 2024/06/10.
//
#include "webserver/webserver.h"

namespace tiny_web_server {
    WebServer::WebServer() {
        //http_conn类对象
        users = new http_conn[MAX_FD];
        //root文件夹路径
        char server_path[200];
        getcwd(server_path, 200);
        char root[6] = "/root";
        m_root = (char *) malloc(strlen(server_path) + strlen(root) + 1);
        strcpy(m_root, server_path);
        strcat(m_root, root);
        //定时器
        users_timer = new ClientData[MAX_FD];
    }

    WebServer::~WebServer() {
        close(m_epollfd);
        close(m_listenfd);
        close(m_pipefd[1]);
        close(m_pipefd[0]);
        delete[] users;
        delete[] users_timer;
        delete m_pool;
    }

    void WebServer::init(int port, std::string user, std::string passWord, std::string databaseName, int log_write,
                         int opt_linger, int trig_mode, int sql_num, int thread_num, int close_log, int actor_model) {
        m_port = port;
        m_user = user;
        m_passWord = passWord;
        m_databaseName = databaseName;
        m_sql_num = sql_num;
        m_thread_num = thread_num;
        m_log_write = log_write;
        m_OPT_LINGER = opt_linger;
        m_TRIGMode = trig_mode;
        m_close_log = close_log;
        m_actormodel = actor_model;
    }

    void WebServer::trig_mode() {
        //LT + LT
        if (0 == m_TRIGMode) {
            m_LISTENTrigmode = 0;
            m_CONNTrigmode = 0;
        }
            //LT + ET
        else if (1 == m_TRIGMode) {
            m_LISTENTrigmode = 0;
            m_CONNTrigmode = 1;
        }
            //ET + LT
        else if (2 == m_TRIGMode) {
            m_LISTENTrigmode = 1;
            m_CONNTrigmode = 0;
        }
            //ET + ET
        else if (3 == m_TRIGMode) {
            m_LISTENTrigmode = 1;
            m_CONNTrigmode = 1;
        }
    }

    void WebServer::log_write() {
        if (0 == m_close_log) {
            //初始化日志
            if (1 == m_log_write)
                Log::get_instance()->init("./ServerLog", m_close_log, 2000, 800000, 800);
            else
                Log::get_instance()->init("./ServerLog", m_close_log, 2000, 800000, 0);
        }
    }

    void WebServer::sql_pool() {
        //初始化数据库连接池
        m_connPool = ConnectionPool::getInstance();
        m_connPool->initConnectionPool("localhost", m_user, m_passWord, m_databaseName, 3306, m_sql_num, m_close_log);
        //初始化数据库读取表
        users->initmysql_result(m_connPool);
    }

    void WebServer::thread_pool() {
        m_pool = new ThreadPool<http_conn>(m_actormodel, m_connPool, m_thread_num);
    }

    void WebServer::eventListen() {
        //网络编程基础步骤
        m_listenfd = socket(PF_INET, SOCK_STREAM, 0);
        assert(m_listenfd >= 0);
        //优雅关闭连接
        if (0 == m_OPT_LINGER) {
            struct linger tmp = {0, 1};
            setsockopt(m_listenfd, SOL_SOCKET, SO_LINGER, &tmp, sizeof(tmp));
        } else if (1 == m_OPT_LINGER) {
            struct linger tmp = {1, 1};
            setsockopt(m_listenfd, SOL_SOCKET, SO_LINGER, &tmp, sizeof(tmp));
        }
        int ret = 0;
        struct sockaddr_in address;
        bzero(&address, sizeof(address));
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = htonl(INADDR_ANY);
        address.sin_port = htons(m_port);
        int flag = 1;
        setsockopt(m_listenfd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));
        ret = bind(m_listenfd, (struct sockaddr *) &address, sizeof(address));
        assert(ret >= 0);
        ret = listen(m_listenfd, 5);
        assert(ret >= 0);
        utils.init(TIMESLOT);
        //epoll创建内核事件表
        epoll_event events[MAX_EVENT_NUMBER];
        m_epollfd = epoll_create(5);
        assert(m_epollfd != -1);
        utils.addFd(m_epollfd, m_listenfd, false, m_LISTENTrigmode);
        http_conn::m_epollfd = m_epollfd;
        ret = socketpair(PF_UNIX, SOCK_STREAM, 0, m_pipefd);
        assert(ret != -1);
        utils.setNonBlocking(m_pipefd[1]);
        utils.addFd(m_epollfd, m_pipefd[0], false, 0);
        utils.addSig(SIGPIPE, SIG_IGN);
        utils.addSig(SIGALRM, utils.sigHandler, false);
        utils.addSig(SIGTERM, utils.sigHandler, false);
        alarm(TIMESLOT);
        //工具类,信号和描述符基础操作
        Utils::u_pipe_fd = m_pipefd;
        Utils::u_epoll_fd = m_epollfd;
    }

    void WebServer::timer(int conn_fd, struct sockaddr_in client_address) {
        users[conn_fd].init(conn_fd, client_address, m_root, m_CONNTrigmode, m_close_log, m_user, m_passWord, m_databaseName);
        //初始化client_data数据
        //创建定时器，设置回调函数和超时时间，绑定用户数据，将定时器添加到链表中
        users_timer[conn_fd].address = client_address;
        users_timer[conn_fd].sock_fd = conn_fd;
        Timer *timer = new Timer();
        timer->user_data = &users_timer[conn_fd];
        timer->cb_func = cb_func;
        time_t cur = time(NULL);
        timer->expire = cur + 3 * TIMESLOT;
        users_timer[conn_fd].timer = timer;
        utils.sortTimerList.push(timer);
    }

//若有数据传输，则将定时器往后延迟3个单位
//并对新的定时器在链表上的位置进行调整
    void WebServer::adjust_timer(Timer *timer) {
        time_t cur = time(NULL);
        timer->expire = cur + 3 * TIMESLOT;
        utils.sortTimerList.push(timer);
        LOG_INFO("%s", "adjust timer once");
    }

    void WebServer::deal_timer(Timer *timer, int sock_fd) {
        timer->cb_func(&users_timer[sock_fd]);
        if (timer) utils.sortTimerList.remove(timer);
        LOG_INFO("close fd %d", users_timer[sock_fd].sock_fd);
    }

    bool WebServer::dealClientData() {
        struct sockaddr_in client_address;
        socklen_t client_addrlength = sizeof(client_address);
        if (0 == m_LISTENTrigmode) {
            int connfd = accept(m_listenfd, (struct sockaddr *) &client_address, &client_addrlength);
            if (connfd < 0) {
                LOG_ERROR("%s:errno is:%d", "accept error", errno);
                return false;
            }
            if (http_conn::m_user_count >= MAX_FD) {
                utils.showError(connfd, "Internal server busy");
                LOG_ERROR("%s", "Internal server busy");
                return false;
            }
            timer(connfd, client_address);
        } else {
            while (1) {
                int connfd = accept(m_listenfd, (struct sockaddr *) &client_address, &client_addrlength);
                if (connfd < 0) {
                    LOG_ERROR("%s:errno is:%d", "accept error", errno);
                    break;
                }
                if (http_conn::m_user_count >= MAX_FD) {
                    utils.showError(connfd, "Internal server busy");
                    LOG_ERROR("%s", "Internal server busy");
                    break;
                }
                timer(connfd, client_address);
            }
            return false;
        }
        return true;
    }

    bool WebServer::dealWithSignal(bool &timeout, bool &stop_server) {
        int ret = 0;
        int sig;
        char signals[1024];
        ret = recv(m_pipefd[0], signals, sizeof(signals), 0);
        if (ret == -1) {
            return false;
        } else if (ret == 0) {
            return false;
        } else {
            for (int i = 0; i < ret; ++i) {
                switch (signals[i]) {
                    case SIGALRM: {
                        timeout = true;
                        break;
                    }
                    case SIGTERM: {
                        stop_server = true;
                        break;
                    }
                }
            }
        }
        return true;
    }

    void WebServer::dealWithRead(int sock_fd) {
        Timer *timer = users_timer[sock_fd].timer;
        //reactor
        if (1 == m_actormodel) {
            if (timer) {
                adjust_timer(timer);
            }
            //若监测到读事件，将该事件放入请求队列
            m_pool->append(users + sock_fd, 0);
            while (true) {
                if (1 == users[sock_fd].improv) {
                    if (1 == users[sock_fd].timer_flag) {
                        deal_timer(timer, sock_fd);
                        users[sock_fd].timer_flag = 0;
                    }
                    users[sock_fd].improv = 0;
                    break;
                }
            }
        } else {
            //proactor
            if (users[sock_fd].read_once()) {
                LOG_INFO("deal with the client(%s)", inet_ntoa(users[sock_fd].get_address()->sin_addr));
                //若监测到读事件，将该事件放入请求队列
                m_pool->append_p(users + sock_fd);
                if (timer) {
                    adjust_timer(timer);
                }
            } else {
                deal_timer(timer, sock_fd);
            }
        }
    }

    void WebServer::dealWithWrite(int sock_fd) {
        Timer *timer = users_timer[sock_fd].timer;
        //reactor
        if (1 == m_actormodel) {
            if (timer) {
                adjust_timer(timer);
            }
            m_pool->append(users + sock_fd, 1);
            while (true) {
                if (1 == users[sock_fd].improv) {
                    if (1 == users[sock_fd].timer_flag) {
                        deal_timer(timer, sock_fd);
                        users[sock_fd].timer_flag = 0;
                    }
                    users[sock_fd].improv = 0;
                    break;
                }
            }
        } else {
            //proactor
            if (users[sock_fd].write()) {
                LOG_INFO("send data to the client(%s)", inet_ntoa(users[sock_fd].get_address()->sin_addr));
                if (timer) {
                    adjust_timer(timer);
                }
            } else {
                deal_timer(timer, sock_fd);
            }
        }
    }

    void WebServer::eventLoop() {
        bool timeout = false;
        bool stop_server = false;
        while (!stop_server) {
            int number = epoll_wait(m_epollfd, events, MAX_EVENT_NUMBER, -1);
            if (number < 0 && errno != EINTR) {
                LOG_ERROR("%s", "epoll failure");
                break;
            }
            for (int i = 0; i < number; i++) {
                int sockfd = events[i].data.fd;
                //处理新到的客户连接
                if (sockfd == m_listenfd) {
                    bool flag = dealClientData();
                    if (false == flag)
                        continue;
                } else if (events[i].events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR)) {
                    //服务器端关闭连接，移除对应的定时器
                    Timer *timer = users_timer[sockfd].timer;
                    deal_timer(timer, sockfd);
                }
                //处理信号
                else if ((sockfd == m_pipefd[0]) && (events[i].events & EPOLLIN)) {
                    bool flag = dealWithSignal(timeout, stop_server);
                    if (false == flag) LOG_ERROR("%s", "deal Client Data failure");
                }
                    //处理客户连接上接收到的数据
                else if (events[i].events & EPOLLIN) {
                    dealWithRead(sockfd);
                } else if (events[i].events & EPOLLOUT) {
                    dealWithWrite(sockfd);
                }
            }
            if (timeout) {
                utils.timerHandler();
                LOG_INFO("%s", "timer tick");
                timeout = false;
            }
        }
    }
}