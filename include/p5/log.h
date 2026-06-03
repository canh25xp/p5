#pragma once

#include <CLI/Error.hpp>

#include <iostream>
#include <sstream>
#include <string>

enum class LogLevel {
    Info = 0,
    Warn = 1,
    Error = 2,
    Fatal = 3
};

enum class LogSource {
    CLI,
    App,
    API
};

class Log {
public:
    static const char *ColorRed;
    static const char *ColorYellow;
    static const char *ColorGreen;
    static const char *ColorNormal;

    static LogLevel CurrentLevel;
    static int ConciseLevel;

    static void DisableColoredOutput();

    static void Output(LogLevel level, LogSource source, const char *func, int line, const std::string &msg);
    static void Print(const std::string &msg);
};

#define LOG_INTERNAL(level, source, x)                                  \
    do {                                                                \
        if (level >= Log::CurrentLevel) {                               \
            std::ostringstream _oss;                                    \
            _oss << x;                                                  \
            Log::Output(level, source, __func__, __LINE__, _oss.str()); \
        }                                                               \
    } while (0)

#define INFO(x) LOG_INTERNAL(LogLevel::Info, LogSource::App, x)
#define WARN(x) LOG_INTERNAL(LogLevel::Warn, LogSource::App, x)
#define ERROR(x) LOG_INTERNAL(LogLevel::Error, LogSource::App, x)
#define FATAL(x)                                          \
    do {                                                  \
        LOG_INTERNAL(LogLevel::Fatal, LogSource::App, x); \
        exit(1);                                          \
    } while (0)

#define CLI_INFO(x) LOG_INTERNAL(LogLevel::Info, LogSource::CLI, x)
#define CLI_WARN(x) LOG_INTERNAL(LogLevel::Warn, LogSource::CLI, x)
#define CLI_ERROR(x)                                      \
    do {                                                  \
        LOG_INTERNAL(LogLevel::Error, LogSource::CLI, x); \
        throw CLI::RuntimeError(1);                       \
    } while (0)
#define CLI_FATAL(x)                                      \
    do {                                                  \
        LOG_INTERNAL(LogLevel::Fatal, LogSource::CLI, x); \
        exit(1);                                          \
    } while (0)

#define API_INFO(x) LOG_INTERNAL(LogLevel::Info, LogSource::API, x)
#define API_WARN(x) LOG_INTERNAL(LogLevel::Warn, LogSource::API, x)
#define API_ERROR(x) LOG_INTERNAL(LogLevel::Error, LogSource::API, x)
#define API_FATAL(x)                                      \
    do {                                                  \
        LOG_INTERNAL(LogLevel::Fatal, LogSource::API, x); \
        exit(1);                                          \
    } while (0)

#define PRINT(x)                 \
    do {                         \
        std::ostringstream _oss; \
        _oss << x;               \
        Log::Print(_oss.str());  \
    } while (0)
