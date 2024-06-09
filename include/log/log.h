//
// Created by zpx on 2024/06/08.
//

#ifndef TINYWEBSERVER_LOG_H
#define TINYWEBSERVER_LOG_H

#include "utils/block_queue.h"
#include <thread>
#include <cstring>
#include <cstdarg>
#define MAX_NAME_SIZE 128
namespace tiny_web_server {
    class Log {
    public:
        /**
         * 单例实例化日志
         * @return
         */
        static Log *get_instance();
        /**
         * 将缓冲区的文件写入日志
         * @param args
         * @return
         */
        static void flush_log_thread();
        //可选择的参数有日志文件、日志缓冲区大小、最大行数以及最长日志条队列
        /**
         * 初始化日志
         * @param file_name 文件名
         * @param close_log 是否关闭日志
         * @param log_buf_size 缓冲区大小
         * @param split_lines 划分行的数量
         * @param max_queue_size 缓冲队列大小
         * @return
         */
        bool init(const char *file_name, bool close_log, int log_buf_size = 8192, int split_lines = 5000000, int max_queue_size = 0);
        /**
         * 写日志
         * @param level 日志类别
         * @param format 格式
         * @param ...
         */
        void write_log(int level, const char *format, ...);
        /**
         *
         */
        void flush();
    private:
        Log();
        virtual~Log();
        /**
         * 异步写入日志
         * @return
         */
        void *async_write_log();
    private:
        char dir_name_[MAX_NAME_SIZE];  // 路径名
        char log_name_[MAX_NAME_SIZE];  // 文件名
        int m_split_lines_;             // 日志最大行数
        int m_log_buf_size_;            // 日志缓冲区大小
        long long m_count_;             // 日志行数记录
        int m_today_;                    // 因为按天分类,记录当前时间是那一天
        FILE *m_fp_;                     // 打开log的文件指针
        char *m_buf_;
        std::shared_ptr<BlockQueue<std::string>> m_log_queue_; //阻塞队列
        bool m_is_async_;                  //是否同步标志位
        std::mutex m_mutex_;
        bool m_close_log_;                 //关闭日志
    };

#define LOG_DEBUG(format, ...)  {Log::get_instance()->write_log(0, format, ##__VA_ARGS__); Log::get_instance()->flush();}
#define LOG_INFO(format, ...)   {Log::get_instance()->write_log(1, format, ##__VA_ARGS__); Log::get_instance()->flush();}
#define LOG_WARN(format, ...)   {Log::get_instance()->write_log(2, format, ##__VA_ARGS__); Log::get_instance()->flush();}
#define LOG_ERROR(format, ...)  {Log::get_instance()->write_log(3, format, ##__VA_ARGS__); Log::get_instance()->flush();}
}
#endif //TINYWEBSERVER_LOG_H
