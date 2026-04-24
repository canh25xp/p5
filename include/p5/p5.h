#pragma once

#include <string>
#include <vector>

#include "commands/result.h"
#include "p4/clientapi.h"

#include "clients.h"
#include "users.h"
#include "clients.h"

class P5 {
    ClientApi m_ClientAPI;
    int m_Usage;
    bool m_LibrariesInitialized;
    bool m_AutoResolving{false};

    bool Initialize();
    bool Deinitialize();
    bool Reinitialize();
    bool CheckErrors(Error &e, StrBuf &msg);

    // WARN: this call p4 clients with "tag" protocol before any other commands.
    // Result in every commands afterward force running with protocol tag (Ugly output if the commands not yet defined Output for Result)
    // TODO: Solutions:
    // - Reset protocol after done. Not possible since SetProtocol() must happen _before_ the Init() hence cannot be changed afterward.
    // - Set protocol tags by default and accept breaking compatibility with p4
    // - Set protocol by default and define output for all p4 commands. Costly but doable.
    // - Run AutoResolveClient in a separate p5 instance.
    // - Use a transient ClientApi instance within AutoResolveClient to avoid polluting the main instance.
    // - Move auto-resolution to a pre-initialization step before m_ClientAPI.Init() is called.
    // - Implement a smart Result handler that can format tagged output back to plain text for generic commands.
    void AutoResolveClient();

    static const int COMMAND_RETRIES = 3; // Specify how many times a command should be retried before the process exits in a failure.
    static const int COMMAND_REFRESH_THRESHOLD = 100; // Specify how many times a connection should be reused before it is refreshed.

    template <class T>
    T Run(const char *command, const std::vector<std::string> &stringArguments, const int commandRetries = COMMAND_RETRIES);

public:
    static bool InitializeLibraries();
    static bool ShutdownLibraries();

    P5();
    ~P5();

    Result Run(const char *command, int argc, char **argv);
    Result Run(const std::string &commandLine);
    Users ListUsers(const std::vector<std::string> &extraArgs = {});
    Clients ListClients(const std::vector<std::string> &extraArgs = {});
};
