#include "rocket/common/log.h"
#include "rocket/common/util.h"
#include "rocket/common/config.h"
#include "rocket/common/mutex.h"
#include <sys/time.h>
#include <sstream>
#include <stdio.h>

namespace rocket
{
    static Logger *g_logger = nullptr;

    Logger *Logger::GetGlobalLogger()
    {
        return g_logger;
    }

    void Logger::InitGlobalLogger()
    {
        LogLevel global_log_level = StringToLoglevel(Config::GetGlobalConfig()->m_log_level);
        g_logger = new Logger(global_log_level);
    }

    std::string LogLevelToString(LogLevel level)
    {
        switch (level)
        {
        case Debug:
            return "DEBUG";
        case Info:
            return "INFO ";
        case Error:
            return "ERROR";
        default:
            return "UNKNOW";
        }
    }

    LogLevel StringToLoglevel(const std::string log_level)
    {
        if (log_level == "DEBUG")
        {
            return LogLevel::Debug;
        }
        else if (log_level == "INFO")
        {
            return LogLevel::Info;
        }
        else if (log_level == "ERROR")
        {
            return LogLevel::Error;
        }
        else
        {
            return LogLevel::Unknow;
        }
    }

    std::string LogEvent::toString()
    {
        struct timeval now_time;
        gettimeofday(&now_time, nullptr);
        struct tm now_time_t;
        localtime_r(&(now_time.tv_sec), &now_time_t);
        char buf[128];
        strftime(&buf[0], 128, "%Y-%m-%d %H:%M:%S", &now_time_t);
        std::string time_str(buf);

        int ms = now_time.tv_usec / 1000;
        time_str = time_str + "." + std::to_string(ms);

        m_pid = getPid();
        m_thread_id = getThreadId();

        std::stringstream ss;
        ss << "[" << LogLevelToString(m_level) << "] "
           << "[" << time_str << "] "
           << "[" << m_pid << ":" << m_thread_id << "] ";

        return ss.str();
    }

    void Logger::pushLog(const std::string &msg)
    {
        ScopeMutex<Mutex> lock(m_mutex);
        m_buffer.push(msg);
        lock.unlock(); // 不解锁也没关系，这段执行完会析构掉
    }

    void Logger::log()
    {
        ScopeMutex<Mutex> lock(m_mutex);
        std::queue<std::string> tmp = m_buffer;
        m_buffer.swap(tmp);
        lock.unlock(); // buffer最后是要被清空的

        while (!tmp.empty())
        {
            std::string msg = tmp.front();
            tmp.pop();
            printf(msg.c_str(), nullptr);
        }
    }
}
