#pragma once

#include <string>
#include <utility>
#include <vector>

#include "commands/result.h"
#include "p4/clientapi.h"

#include "users_result.h"
#include "info_result.h"
#include "test_result.h"

class P5 {
    ClientApi m_ClientAPI;
    int m_Usage;

    bool Initialize();
    bool Deinitialize();
    bool Reinitialize();
    bool CheckErrors(Error &e, StrBuf &msg);

    static const int COMMAND_RETRIES = 1;
    static const int COMMAND_REFRESH_THRESHOLD = 1;

    template <class T>
    T Run(const char *command, const std::vector<std::string> &stringArguments, const int commandRetries = COMMAND_RETRIES);

public:
    static bool InitializeLibraries();
    static bool ShutdownLibraries();

    P5();
    ~P5();

    Result Run(const char *command, int argc, char **argv);
    Result Run(const std::string &commandLine);
    TestResult Test(const int retries = 5);
    UsersResult Users(const std::vector<std::string> &extraArgs = {});
    InfoResult Info();
};
