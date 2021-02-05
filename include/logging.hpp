#ifndef LOGGING_HPP
#define LOGGING_HPP

#include <iostream>
#include <sstream>
#include <cstdlib>

enum LogLevel
{
    DEBUG = 0,
    INFO,
    WARNING,
    ERROR
};
struct LocationInfo
{
    LocationInfo(std::string file, int line, const char *func) : file_(file), line_(line), func_(func) {}
    ~LocationInfo() = default;

    std::string file_;
    int line_;
    const char *func_;
};
class LogStream;
class LogWriter;

class LogWriter
{
public:
    LogWriter(LocationInfo location, LogLevel loglevel)
        : location_(location), log_level_(loglevel)
    {
        char *logv = std::getenv("LOGV");
        if (logv)
        {
            std::string string_logv = logv;
            env_log_level = std::stoi(logv);
        }
        else
        {
            env_log_level = 4;
        }
    };

    void operator<(const LogStream &stream);

private:
    void output_log(const std::ostringstream &g);
    LocationInfo location_;
    LogLevel log_level_;
    int env_log_level;
};

class LogStream
{
public:
    LogStream() { sstream_ = new std::stringstream(); }
    ~LogStream() = default;

    template <typename T>
    LogStream &operator<<(const T &val) noexcept
    {
        (*sstream_) << val;
        return *this;
    }

    friend class LogWriter;

private:
    std::stringstream *sstream_;
};

std::string level2string(LogLevel level);
std::string get_short_name(const char *file_path);

#define __FILESHORTNAME__ get_short_name(__FILE__)
#define LOG_IF(level) \
    LogWriter(LocationInfo(__FILESHORTNAME__, __LINE__, __FUNCTION__), level) < LogStream()
#define LOG(level) LOG_##level
#define LOG_DEBUG LOG_IF(DEBUG)
#define LOG_INFO LOG_IF(INFO)
#define LOG_WARNING LOG_IF(WARNING)
#define LOG_ERROR LOG_IF(ERROR)

#endif
