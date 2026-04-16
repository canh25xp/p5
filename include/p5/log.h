#pragma once

#include <iostream>

class Log {
public:
    static const char *ColorRed;
    static const char *ColorYellow;
    static const char *ColorGreen;
    static const char *ColorNormal;

    static void DisableColoredOutput();
};

#define ERROR(x) std::cerr << Log::ColorRed                                        \
                           << "[ ERROR @ " << __func__ << ":" << __LINE__ << " ] " \
                           << x << Log::ColorNormal << std::endl

#ifdef P5_VERBOSE_LOGGING
#define PRINT(x) std::cout << "[ PRINT @ " << __func__ << ":" << __LINE__ << " ] " << x << std::endl

#define WARN(x) std::cerr << Log::ColorYellow                                    \
                          << "[ WARN @ " << __func__ << ":" << __LINE__ << " ] " \
                          << x << Log::ColorNormal << std::endl

#define INFO(x) std::cerr << Log::ColorGreen                                     \
                          << "[ INFO @ " << __func__ << ":" << __LINE__ << " ] " \
                          << x << Log::ColorNormal << std::endl
#else
#define PRINT(x) ((void)0)
#define WARN(x) ((void)0)
#define INFO(x) ((void)0)
#endif
