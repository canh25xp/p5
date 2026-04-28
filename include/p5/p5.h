#pragma once

#include <string>
#include <vector>

#include <p4/clientapi.h>

#include "commands/result.h"
#include "commands/clients.h"
#include "commands/users.h"

class P5 {
    ClientApi m_ClientAPI;
    int m_Usage;
    bool m_LibrariesInitialized;

    bool Initialize();
    bool Deinitialize();
    bool Reinitialize();
    bool CheckErrors(Error &e, StrBuf &msg);
    ClientUser &Execute(const char *command, std::vector<char *> &argv, ClientUser &user);

    static const int COMMAND_RETRIES = 3;             // Specify how many times a command should be retried before the process exits in a failure.
    static const int COMMAND_REFRESH_THRESHOLD = 100; // Specify how many times a connection should be reused before it is refreshed.

    template <class T>
    T Run(const char *command, const std::vector<std::string> &stringArguments, const int commandRetries = COMMAND_RETRIES);

public:
    static bool InitializeLibraries();
    static bool ShutdownLibraries();

    P5();
    ~P5();

    Result Run(const std::string &command, const std::vector<std::string> &args);
    Result Run(const std::string &commandLine);
    Users ListUsers(const std::vector<std::string> &extraArgs = {});
    Clients ListClients(const std::vector<std::string> &extraArgs = {});
};
