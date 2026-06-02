#pragma once

#include <string>
#include <vector>

#include <p4/clientapi.h>

#include "commands/result.h"
#include "commands/changes.h"
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
    ClientUser &Execute(const std::string &command, const std::vector<std::string> &argv, ClientUser &user);
    void RefreshIfNeeded();
    std::string AutoResolve();
    // Using ephemeral ClientApi with tag protocol only on temp connection.
    // We will use RunClients directly if all commands is implemented with tag protocol.
    // Until then, we use this as a work-around
    Clients FetchClientsTagged(const std::vector<std::string> &args);

    static const int COMMAND_RETRIES = 0;             // Specify how many times a command should be retried before the process exits in a failure.
    static const int COMMAND_REFRESH_THRESHOLD = 100; // Specify how many times a connection should be reused before it is refreshed.

    template <class T>
    T Run(const std::string &command, const std::vector<std::string> &args, const int commandRetries = COMMAND_RETRIES);

public:
    static bool InitializeLibraries();
    static bool ShutdownLibraries();

    P5();
    ~P5();

    Result Run(const std::string &command, const std::vector<std::string> &args);
    Result Run(const std::string &commandLine);

    /// Run a command with a custom ClientUser (e.g. tag collectors).
    void RunWithUser(const std::string &command, const std::vector<std::string> &args, ClientUser &user);

    void SetTagProtocol();
    Users RunUsers(const std::vector<std::string> &args = {});
    Clients RunClients(const std::vector<std::string> &args = {});
    Changes RunChanges(const std::vector<std::string> &args = {});
};
