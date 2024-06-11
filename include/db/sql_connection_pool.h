//
// Created by zpx on 2024/06/08.
//

#ifndef TINYWEBSERVER_SQL_CONNECTION_POOL_H
#define TINYWEBSERVER_SQL_CONNECTION_POOL_H

#include <stdio.h>
#include <list>
#include "wrap_mysql.h"
#include <error.h>
#include <string.h>
#include <iostream>
#include <string>
#include <condition_variable>

namespace tiny_web_server {
    class ConnectionPool {
    public:
        WrapMysql *getConnection();
        bool releaseConnection(WrapMysql *connection);
        int getFreeConnection();
        void destroyConnection();
        static ConnectionPool *getInstance();
        void initConnectionPool(std::string url, std::string username, std::string password, std::string database_name, int port, int
        max_connections, bool close_log);
    private:
        ConnectionPool();
        ~ConnectionPool();
        int m_maxConnections_;
        int m_currentConnections_;
        int m_freeConnections_;
        std::mutex m_mutex_;
        std::list<WrapMysql *> connect_lists_;
    public:
        std::string m_url_;                 //主机地址
        std::string m_port_;                //数据库端口号
        std::string m_username_;            //登陆数据库用户名
        std::string m_passWord_;            //登陆数据库密码
        std::string m_database_name_;       //使用数据库名
        bool m_close_log_;                  //日志开关
        std::condition_variable m_condition_variable_;
    };
}
#endif //TINYWEBSERVER_SQL_CONNECTION_POOL_H
