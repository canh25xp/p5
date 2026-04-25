#include "log.h"

// g++ -std=c++17 -Iinclude/p5 source/test_log.cpp source/log.cpp -o test_log && ./test_log
int main() {
    Log::CurrentLevel = LogLevel::Info;
    std::cout << "\nCurrent Log Level: " << static_cast<int>(Log::CurrentLevel) << "\n";

    INFO("This is an app INFO message.");
    WARN("This is an app WARN message.");
    ERROR("This is an app ERROR message.");

    CLI_INFO("This is a CLI INFO message.");
    CLI_WARN("This is a CLI WARN message.");
    CLI_ERROR("This is a CLI ERROR message.");

    P4_INFO("This is a P4 INFO message.");
    P4_WARN("This is a P4 WARN message.");
    P4_ERROR("This is a P4 ERROR message.");

    PRINT("This is a PRINT message.");

    Log::CurrentLevel = LogLevel::Warn;
    std::cout << "\nCurrent Log Level: " << static_cast<int>(Log::CurrentLevel) << "\n";
    INFO("This is an app INFO message.");
    WARN("This is an app WARN message.");
    ERROR("This is an app ERROR message.");

    CLI_INFO("This is a CLI INFO message.");
    CLI_WARN("This is a CLI WARN message.");
    CLI_ERROR("This is a CLI ERROR message.");

    P4_INFO("This is a P4 INFO message.");
    P4_WARN("This is a P4 WARN message.");
    P4_ERROR("This is a P4 ERROR message.");

    PRINT("This is a PRINT message.");

    Log::CurrentLevel = LogLevel::Error;
    std::cout << "\nCurrent Log Level: " << static_cast<int>(Log::CurrentLevel) << "\n";
    INFO("This is an app INFO message.");
    WARN("This is an app WARN message.");
    ERROR("This is an app ERROR message.");

    CLI_INFO("This is a CLI INFO message.");
    CLI_WARN("This is a CLI WARN message.");
    CLI_ERROR("This is a CLI ERROR message.");

    P4_INFO("This is a P4 INFO message.");
    P4_WARN("This is a P4 WARN message.");
    P4_ERROR("This is a P4 ERROR message.");

    PRINT("This is a PRINT message.");

    // std::cout << "\n=== Testing Fatal Execution ===\n";
    // FATAL("This is a FATAL message, terminating program.");
    // CLI_FATAL("This is a CLI FATAL message.");
    // P4_FATAL("This is a P4 FATAL message.");
    return 0;
}
