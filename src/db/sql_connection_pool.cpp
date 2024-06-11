//
// Created by zpx on 2024/06/08.
//
#include "db/sql_connection_pool.h"

namespace tiny_web_server {
    ConnectionPool::ConnectionPool() : m_freeConnections_{0}, m_maxConnections_{0}, m_currentConnections_{0} {}

    ConnectionPool *ConnectionPool::getInstance() {
        static ConnectionPool instance;
        return &instance;
    }

    void ConnectionPool::initConnectionPool(std::string url, std::string username, std::string password, std::string database_name, int
    port, int max_connections, bool close_log) {
        m_url_ = url;
        m_username_ = username;
        m_database_name_ = database_name;
        m_port_ = port;
        m_maxConnections_ = max_connections;
        m_close_log_ = close_log;
        for (int i = 0; i < max_connections; i++) {
            WrapMysql *connection = new WrapMysql(false);
            if (!connection->connect(database_name.c_str(), url.c_str(), username.c_str(), password.c_str(), port)) {
                LOG_ERROR("MySQL connection error.");
            }
            connect_lists_.push_back(connection);
            ++m_freeConnections_;
        }
        m_maxConnections_ = m_freeConnections_;
        m_condition_variable_.notify_all();
    }

    WrapMysql *ConnectionPool::getConnection() {
        WrapMysql *connection = nullptr;
        std::unique_lock<std::mutex> lock(m_mutex_);
        m_condition_variable_.wait(lock, [&] { return connect_lists_.size() > 0; });
        connection = connect_lists_.front();
        connect_lists_.pop_front();
        lock.unlock();
        --m_freeConnections_;
        ++m_currentConnections_;
        return connection;
    }

    bool ConnectionPool::releaseConnection(WrapMysql *connection) {
        if (connection == nullptr)return false;
        {
            std::lock_guard<std::mutex> lock(m_mutex_);
            connect_lists_.push_back(connection);
            ++m_freeConnections_;
            --m_currentConnections_;
        }
        m_condition_variable_.notify_one();
        return true;
    }

    void ConnectionPool::destroyConnection() {
        std::lock_guard<std::mutex> lock(m_mutex_);
        if (connect_lists_.empty())return;
        for (auto &item: connect_lists_) {
            item->disconnect();
        }
        connect_lists_.clear();
        m_freeConnections_ = 0;
        m_currentConnections_ = 0;
        m_condition_variable_.notify_all();
    }

    int ConnectionPool::getFreeConnection() {
        return m_freeConnections_;
    }

    ConnectionPool::~ConnectionPool() {
        destroyConnection();
    }
}