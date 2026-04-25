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

bool Log::ConciseFormat = std::getenv("P5_LOG_CONCISE") != nullptr;

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

    if (ConciseFormat) {
        std::cerr << color << "[" << conciseLevelStr << "][" << sourceStr << "] "
                  << msg << ColorNormal << std::endl;
    } else {
        std::string funcLine = std::string(func) + ":" + std::to_string(line);
        std::cerr << color << "[ " << std::left << std::setw(5) << levelStr << " ][ "
                  << std::left << std::setw(3) << sourceStr << " @ "
                  << std::left << std::setw(15) << funcLine << " ] "
                  << msg << ColorNormal << std::endl;
    }
}

void Log::Print(const std::string &msg) {
    std::cout << msg << std::endl;
}
