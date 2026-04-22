#pragma once

#include <string>
#include <utility>
#include <vector>

#include "commands.h"
#include "commands/result.h"
#include "global_options.h"
#include "p4/clientapi.h"

#include "users.h"

/// Tag for `P5` construction without opening a Helix client connection (CLI setup only).
struct P5ForCliConfig {};

namespace CLI {
class App;
}

class P5 {
    ClientApi m_ClientAPI;
    int m_Usage;
    bool m_LibrariesInitialized;

    GlobalOptions m_globalOptions;
    Commands m_commands;

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

    /// RAII guard: calls InitializeLibraries on construction, ShutdownLibraries on destruction.
    class LibrariesGuard {
    public:
        LibrariesGuard();
        ~LibrariesGuard();
        LibrariesGuard(const LibrariesGuard &) = delete;
        LibrariesGuard &operator=(const LibrariesGuard &) = delete;
        bool initialized() const;

    private:
        bool m_initialized;
    };

    P5();
    explicit P5(P5ForCliConfig);
    ~P5();

    void register_cli(CLI::App &app);

    Result Run(const char *command, int argc, char **argv);
    Result Run(const std::string &commandLine);
    Users ListUsers(const std::vector<std::string> &extraArgs = {});
};
