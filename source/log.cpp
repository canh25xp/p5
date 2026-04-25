#include "log.h"
#include <cstdlib>
#include <iomanip>

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

int Log::ConciseLevel = []() {
    const char *env = std::getenv("P5_LOG_CONCISE");
    if (env == nullptr) {
        return 0;
    }
    try {
        return std::stoi(env);
    } catch (...) {
        return 1;
    }
}();

void Log::DisableColoredOutput() {
    ColorRed = COLOR_NORMAL;
    ColorYellow = COLOR_NORMAL;
    ColorGreen = COLOR_NORMAL;
    ColorNormal = COLOR_NORMAL;
}

void Log::Output(LogLevel level, LogSource source, const char *func, int line, const std::string &msg) {
    const char *color = ColorNormal;
    const char *levelStr = "UNKNOWN";
    const char *conciseLevelStr = "U";
    switch (level) {
    case LogLevel::Info:
        color = ColorGreen;
        levelStr = "INFO";
        conciseLevelStr = "I";
        break;
    case LogLevel::Warn:
        color = ColorYellow;
        levelStr = "WARN";
        conciseLevelStr = "W";
        break;
    case LogLevel::Error:
        color = ColorRed;
        levelStr = "ERROR";
        conciseLevelStr = "E";
        break;
    case LogLevel::Fatal:
        color = ColorRed;
        levelStr = "FATAL";
        conciseLevelStr = "F";
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
    case LogSource::API:
        sourceStr = "API";
        break;
    }

    if (ConciseLevel == 1) {
        std::cerr << color << "[" << conciseLevelStr << "][" << sourceStr << "] "
                  << msg << ColorNormal << std::endl;
    } else if (ConciseLevel == 2) {
        std::cerr << color << "[" << conciseLevelStr << "]"
                  << msg << ColorNormal << std::endl;
    } else if (ConciseLevel >= 3) {
        std::cerr << color << msg << ColorNormal << std::endl;
    } else {
        std::string funcLine = std::string(func) + ":" + std::to_string(line);
        std::cerr << color << "[ " << std::left << std::setw(5) << levelStr << " ][ "
                  << std::left << std::setw(3) << sourceStr << " @ "
                  << std::left << std::setw(8) << funcLine << " ] " // this assuming no line of code exceed 999 lines (which is insane)
                  << msg << ColorNormal << std::endl;
    }
}

void Log::Print(const std::string &msg) {
    std::cout << msg << std::endl;
}
