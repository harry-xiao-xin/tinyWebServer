////
//// Created by zpx on 2024/06/08.
////
//#include "db/sql_connection_pool.h"
//#include <gtest/gtest.h>
//#include <glog/logging.h>
//
//const std::string url = "localhost";
//const std::string user_name = "root";
//const std::string password = "123456";
//const std::string database_name = "test";
//const int port = 3306;
//const int max_connections = 3;
//using namespace tiny_web_server;
//TEST(test_sql_connection_pool, test_conect) {
//    ConnectionPool *connection = ConnectionPool::getInstance();
//    connection->initConnectionPool(url, user_name, password, database_name, port, max_connections, false);
//    ASSERT_EQ(connection->getFreeConnection(), max_connections);
//    connection->destroyConnection();
//}
//
//TEST(test_sql_connection_pool, test_insert) {
//    ConnectionPool *connection = ConnectionPool::getInstance();
//    connection->initConnectionPool(url, user_name, password, database_name, port, max_connections, false);
//    ASSERT_EQ(connection->getFreeConnection(), max_connections);
//    auto item = connection->getConnection();
//    ASSERT_EQ(item->insert_exec("insert into test values(%d,%d,%d,%d)", nullptr, 2, 3, 4), true);
//    auto results = item->select_exec("select *from test", nullptr);
//    for (auto &lines: results) {
//        for (auto &row: lines) {
//            std::cout << row << " ";
//        }
//        std::cout << std::endl;
//    }
//    connection->destroyConnection();
//}
//
//TEST(test_sql_connection_pool, test_query) {
//    ConnectionPool *connection = ConnectionPool::getInstance();
//    connection->initConnectionPool(url, user_name, password, database_name, port, max_connections, false);
//    ASSERT_EQ(connection->getFreeConnection(), max_connections);
//    auto item = connection->getConnection();
//    auto results = item->select_exec("select * from test", nullptr);
//    for (auto &lines: results) {
//        for (auto &row: lines) {
//            std::cout << row << " ";
//        }
//        std::cout << std::endl;
//    }
//    connection->destroyConnection();
//}
//
//TEST(test_sql_connection_pool, test_delete) {
//    ConnectionPool *connection = ConnectionPool::getInstance();
//    connection->initConnectionPool(url, user_name, password, database_name, port, max_connections, false);
//    ASSERT_EQ(connection->getFreeConnection(), max_connections);
//    auto item = connection->getConnection();
//    ASSERT_EQ(item->insert_exec("insert into test values(%d,%d,%d,%d)", nullptr, 2, 3, 1), true);
//    std::cout << "Before delete：" << std::endl;
//    auto results = item->select_exec("select * from test", nullptr);
//    for (auto &lines: results) {
//        for (auto &row: lines) {
//            std::cout << row << " ";
//        }
//        std::cout << std::endl;
//    }
//    std::cout << "After delete：" << std::endl;
//    ASSERT_EQ(item->delete_exec("delete from test where score=%d", 1), true);
//    results = item->select_exec("select * from test", nullptr);
//    for (auto &lines: results) {
//        for (auto &row: lines) {
//            std::cout << row << " ";
//        }
//        std::cout << std::endl;
//    }
//    connection->destroyConnection();
//}
//
//TEST(test_sql_connection_pool, test_update) {
//    ConnectionPool *connection = ConnectionPool::getInstance();
//    connection->initConnectionPool(url, user_name, password, database_name, port, max_connections, false);
//    ASSERT_EQ(connection->getFreeConnection(), max_connections);
//    auto item = connection->getConnection();
//    ASSERT_EQ(item->update_exec("update test set score=%d where id=%d", 4, 2), true);
//    auto results = item->select_exec("select * from test", nullptr);
//    for (auto &lines: results) {
//        for (auto &row: lines) {
//            std::cout << row << " ";
//        }
//        std::cout << std::endl;
//    }
//    connection->destroyConnection();
//}
//
//TEST(test_sql_connection_pool, test_release) {
//    ConnectionPool *connection = ConnectionPool::getInstance();
//    connection->initConnectionPool(url, user_name, password, database_name, port, max_connections, false);
//    ASSERT_EQ(connection->getFreeConnection(), max_connections);
//    auto item = connection->getConnection();
//    connection->releaseConnection(item);
//    ASSERT_EQ(connection->getFreeConnection(), max_connections);
//    connection->destroyConnection();
//}
//
//TEST(test_sql_connection_pool, test_get_freeConnection) {
//    ConnectionPool *connection = ConnectionPool::getInstance();
//    connection->initConnectionPool(url, user_name, password, database_name, port, max_connections, false);
//    ASSERT_EQ(connection->getFreeConnection(), max_connections);
//    auto item = connection->getConnection();
//    ASSERT_EQ(connection->getFreeConnection(), max_connections - 1);
//    connection->releaseConnection(item);
//    ASSERT_EQ(connection->getFreeConnection(), max_connections);
//    connection->destroyConnection();
//}