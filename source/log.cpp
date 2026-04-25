#include "log.h"

#define COLOR_RED "\033[91m"
#define COLOR_YELLOW "\033[93m"
#define COLOR_GREEN "\033[32m"
#define COLOR_NORMAL "\033[0m"

const char *Log::ColorRed = COLOR_RED;
const char *Log::ColorYellow = COLOR_YELLOW;
const char *Log::ColorGreen = COLOR_GREEN;
const char *Log::ColorNormal = COLOR_NORMAL;

#ifdef P5_VERBOSE_LOGGING
LogLevel Log::CurrentLevel = LogLevel::Info;
#else
LogLevel Log::CurrentLevel = LogLevel::Error;
#endif

void Log::DisableColoredOutput() {
    ColorRed = COLOR_NORMAL;
    ColorYellow = COLOR_NORMAL;
    ColorGreen = COLOR_NORMAL;
    ColorNormal = COLOR_NORMAL;
}

void Log::Output(LogLevel level, LogSource source, const char *func, int line, const std::string &msg) {
    const char *color = ColorNormal;
    const char *levelStr = "UNKNOWN";
    switch (level) {
    case LogLevel::Info:
        color = ColorGreen;
        levelStr = "INFO";
        break;
    case LogLevel::Warn:
        color = ColorYellow;
        levelStr = "WARN";
        break;
    case LogLevel::Error:
        color = ColorRed;
        levelStr = "ERROR";
        break;
    case LogLevel::Fatal:
        color = ColorRed;
        levelStr = "FATAL";
        break;
    }

    const char *sourceStr = "APP";
    switch (source) {
    case LogSource::CLI:
        sourceStr = "CLI";
        break;
    case LogSource::App:
        sourceStr = "APP";
        break;
    case LogSource::P4:
        sourceStr = "P4";
        break;
    }

    std::cerr << color << "[ " << levelStr << " ][ " << sourceStr << " @ " << func << ":" << line << " ] "
              << msg << ColorNormal << std::endl;
}

void Log::Print(const std::string &msg) {
    std::cout << msg << std::endl;
}
