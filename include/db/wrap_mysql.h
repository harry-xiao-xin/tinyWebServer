//
// Created by zpx on 2024/06/08.
//

#ifndef TINYWEBSERVER_WRAP_MYSQL_H
#define TINYWEBSERVER_WRAP_MYSQL_H

#include <mysql++/mysql++.h>
#include <cstring>
#include "log/log.h"

namespace tiny_web_server {
    class WrapMysql : public mysqlpp::Connection {
    public:
        explicit WrapMysql(bool te = true) : mysqlpp::Connection(te) {}

        template<typename... Args>
        bool insert_exec(const char *format, Args... args);
        template<typename... Args>
        bool update_exec(const char *format, Args... args);
        template<typename... Args>
        bool delete_exec(const char *format, Args... args);
        template<typename... Args>
        mysqlpp::StoreQueryResult select_exec(const char *format, Args... args);
    };

    template<typename...Args>
    bool WrapMysql::insert_exec(const char *format, Args... args) {
        int len_str = std::snprintf(nullptr, 0, format, args...) + 1;
        char *statement = new char[len_str];
        assert(statement != nullptr);
        std::snprintf(statement, len_str, format, args...);
        bool res = query().exec(statement);
        delete[] statement;
        return res;
    }

    template<typename...Args>
    bool WrapMysql::update_exec(const char *format, Args ...args) {
        int len_str = std::snprintf(nullptr, 0, format, args...) + 1;
        char *statement = new char[len_str];
        assert(statement != nullptr);
        std::snprintf(statement, len_str, format, args...);
        bool res = query().exec(statement);
        delete[] statement;
        return res;
    }

    template<typename...Args>
    bool WrapMysql::delete_exec(const char *format, Args ...args) {
        int len_str = std::snprintf(nullptr, 0, format, args...) + 1;
        char *statement = new char[len_str];
        assert(statement != nullptr);
        std::snprintf(statement, len_str, format, args...);
        bool res = query().exec(statement);
        delete[] statement;
        return res;
    }

    template<typename...Args>
    mysqlpp::StoreQueryResult WrapMysql::select_exec(const char *format, Args ...args) {
        int len_str = std::snprintf(nullptr, 0, format, args...) + 1;
        char *statement = new char[len_str];
        assert(statement != nullptr);
        std::snprintf(statement, len_str, format, args...);
        auto result = query(statement).store();
        delete[] statement;
        return result;
    }
}
#endif //TINYWEBSERVER_WRAP_MYSQL_H
