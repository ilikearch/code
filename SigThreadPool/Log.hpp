#pragma once

#include <iostream>
#include <cstdio>
#include <string>
#include <fstream>
#include <sstream>
#include <memory>
#include <filesystem> //C++17
#include <unistd.h>
#include <time.h>
#include "Mutex.hpp"

namespace LogModule
{
    using namespace LockModule;

    // 获取一下当前系统的时间
    std::string CurrentTime()
    {
        time_t time_stamp = ::time(nullptr);
        struct tm curr;
        localtime_r(&time_stamp, &curr); // 时间戳，获取可读性较强的时间信息5

        char buffer[1024];
        // bug
        snprintf(buffer, sizeof(buffer), "%4d-%02d-%02d %02d:%02d:%02d",
                 curr.tm_year + 1900,
                 curr.tm_mon + 1,
                 curr.tm_mday,
                 curr.tm_hour,
                 curr.tm_min,
                 curr.tm_sec);

        return buffer;
    }

    // 构成: 1. 构建日志字符串 2. 刷新落盘(screen, file)
    //  1. 日志文件的默认路径和文件名
    const std::string defaultlogpath = "./log/";
    const std::string defaultlogname = "log.txt";

    // 2. 日志等级
    enum class LogLevel
    {
        DEBUG = 1,
        INFO,
        WARNING,
        ERROR,
        FATAL
    };

    std::string Level2String(LogLevel level)
    {
        switch (level)
        {
        case LogLevel::DEBUG:
            return "DEBUG";
        case LogLevel::INFO:
            return "INFO";
        case LogLevel::WARNING:
            return "WARNING";
        case LogLevel::ERROR:
            return "ERROR";
        case LogLevel::FATAL:
            return "FATAL";
        default:
            return "None";
        }
    }

    // 3. 刷新策略.
    class LogStrategy
    {
    public:
        virtual ~LogStrategy() = default;
        virtual void SyncLog(const std::string &message) = 0;
    };

    // 3.1 控制台策略
    class ConsoleLogStrategy : public LogStrategy
    {
    public:
        ConsoleLogStrategy()
        {
        }
        ~ConsoleLogStrategy()
        {
        }
        void SyncLog(const std::string &message)
        {
            LockGuard lockguard(_lock);
            std::cout << message << std::endl;
        }

    private:
        Mutex _lock;
    };

    // 3.2 文件级(磁盘)策略
    class FileLogStrategy : public LogStrategy
    {
    public:
        FileLogStrategy(const std::string &logpath = defaultlogpath, const std::string &logname = defaultlogname)
            : _logpath(logpath),
              _logname(logname)
        {
            // 确认_logpath是存在的.
            LockGuard lockguard(_lock);

            if (std::filesystem::exists(_logpath))
            {
                return;
            }
            try
            {
                std::filesystem::create_directories(_logpath);
            }
            catch (std::filesystem::filesystem_error &e)
            {
                std::cerr << e.what() << "\n";
            }
        }
        ~FileLogStrategy()
        {
        }
        void SyncLog(const std::string &message)
        {
            LockGuard lockguard(_lock);
            std::string log = _logpath + _logname; // ./log/log.txt
            std::ofstream out(log, std::ios::app); // 日志写入，一定是追加
            if (!out.is_open())
            {
                return;
            }
            out << message << "\n";
            out.close();
        }

    private:
        std::string _logpath;
        std::string _logname;

        // 锁
        Mutex _lock;
    };

    // 日志类: 构建日志字符串, 根据策略，进行刷新
    class Logger
    {
    public:
        Logger()
        {
            // 默认采用ConsoleLogStrategy策略
            _strategy = std::make_shared<ConsoleLogStrategy>();
        }
        void EnableConsoleLog()
        {
            _strategy = std::make_shared<ConsoleLogStrategy>();
        }
        void EnableFileLog()
        {
            _strategy = std::make_shared<FileLogStrategy>();
        }
        ~Logger() {}
        // 一条完整的信息: [2024-08-04 12:27:03] [DEBUG] [202938] [main.cc] [16] + 日志的可变部分(<< "hello world" << 3.14 << a << b;)
        class LogMessage
        {
        public:
            LogMessage(LogLevel level, const std::string &filename, int line, Logger &logger)
                : _currtime(CurrentTime()),
                  _level(level),
                  _pid(::getpid()),
                  _filename(filename),
                  _line(line),
                  _logger(logger)
            {
                std::stringstream ssbuffer;
                ssbuffer << "[" << _currtime << "] "
                         << "[" << Level2String(_level) << "] "
                         << "[" << _pid << "] "
                         << "[" << _filename << "] "
                         << "[" << _line << "] - ";
                _loginfo = ssbuffer.str();
            }
            template <typename T>
            LogMessage &operator<<(const T &info)
            {
                std::stringstream ss;
                ss << info;
                _loginfo += ss.str();
                return *this;
            }

            ~LogMessage()
            {
                if (_logger._strategy)
                {
                    _logger._strategy->SyncLog(_loginfo);
                }
            }

        private:
            std::string _currtime; // 当前日志的时间
            LogLevel _level;       // 日志等级
            pid_t _pid;            // 进程pid
            std::string _filename; // 源文件名称
            int _line;             // 日志所在的行号
            Logger &_logger;       // 负责根据不同的策略进行刷新
            std::string _loginfo;  // 一条完整的日志记录
        };

        // 就是要拷贝,故意的拷贝
        LogMessage operator()(LogLevel level, const std::string &filename, int line)
        {
            return LogMessage(level, filename, line, *this);
        }

    private:
        std::shared_ptr<LogStrategy> _strategy; // 日志刷新的策略方案
    };

    Logger logger;

#define LOG(Level) logger(Level, __FILE__, __LINE__)
#define ENABLE_CONSOLE_LOG() logger.EnableConsoleLog()
#define ENABLE_FILE_LOG() logger.EnableFileLog()
}