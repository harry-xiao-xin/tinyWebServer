//
// Created by zpx on 2024/06/08.
//
#include "log/log.h"

namespace tiny_web_server {
    Log::Log() : m_count_{0}, m_is_async_{false} {}

    Log::~Log() {
        if (m_fp_ != nullptr) {
            fclose(m_fp_);
        }
    }

    Log *Log::get_instance() {
        static Log instance;
        return &instance;
    }

    void Log::flush_log_thread() {
        Log::get_instance()->async_write_log();
    }

    void *Log::async_write_log() {
        std::string single_line;
        while (m_log_queue_->pop(single_line)) {
            std::lock_guard<std::mutex> lock(m_mutex_);
            fputs(single_line.c_str(), m_fp_);
        }
    }

    bool Log::init(const char *file_name, bool close_log, int log_buf_size, int split_lines, int max_queue_size) {
        // 创建异步线程写日志
        if (max_queue_size > 0) {
            m_is_async_ = true;
            m_log_queue_ = std::make_shared<BlockQueue<std::string>>(max_queue_size);
            std::thread t(flush_log_thread);
            t.join();
        }
        m_close_log_ = close_log;
        m_log_buf_size_ = log_buf_size;
        m_buf_ = new char[m_log_buf_size_];
        memset(m_buf_, '\0', m_log_buf_size_);
        m_split_lines_ = split_lines;

        // 获取当前时间
        time_t t = time(nullptr);
        struct tm *sys_time = localtime(&t);
        const char *p = strrchr(file_name, '/');
        char log_full_name[256] = {0};
        if (p == nullptr) {
            snprintf(log_full_name, 255, "%d_%02d_%02d_%s", sys_time->tm_year + 1900, sys_time->tm_mon + 1, sys_time->tm_mday, file_name);
        } else {
            strcpy(log_name_, p + 1);
            strncpy(dir_name_, file_name, p - file_name + 1);
            snprintf(log_full_name, 255, "%s%d_%02d_%02d_%s", dir_name_, sys_time->tm_year + 1900, sys_time->tm_mon + 1,
                     sys_time->tm_mday, log_name_);
        }
        m_today_ = sys_time->tm_mday;
        m_fp_ = fopen(log_full_name, "a");
        if (m_fp_ == nullptr) {
            return false;
        }
        return true;
    }

    void Log::write_log(int level, const char *format, ...) {
        struct timeval now = {0, 0};
        gettimeofday(&now, nullptr);
        time_t t = now.tv_sec;
        struct tm *sys_tm = localtime(&t);
        struct tm my_tm = *sys_tm;
        char s[16] = {0};
        switch (level) {
            case 0:
                strcpy(s, "[DEBUG]:");
                break;
            case 1:
                strcpy(s, "[INFO]:");
                break;
            case 2:
                strcpy(s, "[WARN]:");
                break;
            case 3:
                strcpy(s, "[ERROR]:");
                break;
            default:
                strcpy(s, "[INFO]:");
                break;
        }
        {
            std::lock_guard<std::mutex> lock(m_mutex_);
            //写入一个log，对m_count++, m_split_lines最大行数
            m_count_++;
            if (m_today_ != my_tm.tm_mday || m_count_ % m_split_lines_ == 0) //everyday log
            {
                char new_log[256] = {0};
                fflush(m_fp_);
                fclose(m_fp_);
                char tail[16] = {0};
                snprintf(tail, 16, "%d_%02d_%02d_", my_tm.tm_year + 1900, my_tm.tm_mon + 1, my_tm.tm_mday);
                if (m_today_ != my_tm.tm_mday) {
                    snprintf(new_log, 255, "%s%s%s", dir_name_, tail, log_name_);
                    m_today_ = my_tm.tm_mday;
                    m_count_ = 0;
                } else {
                    snprintf(new_log, 255, "%s%s%s.%lld", dir_name_, tail, log_name_, m_count_ / m_split_lines_);
                }
                m_fp_ = fopen(new_log, "a");
            }
        }
        va_list args;
        va_start(args, format);
        std::string log_str;
        {
            std::lock_guard<std::mutex> lock(m_mutex_);
            //写入的具体时间内容格式
            int n = snprintf(m_buf_, 48, "%d-%02d-%02d %02d:%02d:%02d.%06ld %s ",
                             my_tm.tm_year + 1900, my_tm.tm_mon + 1, my_tm.tm_mday,
                             my_tm.tm_hour, my_tm.tm_min, my_tm.tm_sec, now.tv_usec, s);
            int m = vsnprintf(m_buf_ + n, m_log_buf_size_ - n - 1, format, args);
            m_buf_[n + m] = '\n';
            m_buf_[n + m + 1] = '\0';
            log_str = m_buf_;
        }
        if (m_is_async_ && !m_log_queue_->full()) {
            m_log_queue_->push(log_str);
        } else {
            std::lock_guard<std::mutex> lock(m_mutex_);
            fputs(log_str.c_str(), m_fp_);
        }
        va_end(args);
    }

    void Log::flush() {
        std::lock_guard<std::mutex> lock(m_mutex_);
        fflush(m_fp_);
    }
}