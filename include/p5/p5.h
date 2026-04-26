#pragma once

#include <string>
#include <vector>

#include "commands/result.h"
#include "p4/clientapi.h"

#include "clients.h"
#include "users.h"
#include "log.h"
#include <chrono>
#include <thread>

class P5 {
    ClientApi m_ClientAPI;
    int m_Usage;
    bool m_LibrariesInitialized;
    bool m_AutoResolving{false};

    bool Initialize();
    bool Deinitialize();
    bool Reinitialize();
    bool CheckErrors(Error &e, StrBuf &msg);
    void AutoResolveClient();

    static const int COMMAND_RETRIES = 3;             // Specify how many times a command should be retried before the process exits in a failure.
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

template <class T>
inline T P5::Run(const char *command, const std::vector<std::string> &stringArguments, const int commandRetries) {
    std::string argsString;
    for (const std::string &stringArg : stringArguments) {
        argsString = argsString + " " + stringArg;
    }

    std::vector<char *> argsCharArray;
    for (const std::string &arg : stringArguments) {
        argsCharArray.push_back((char *)arg.c_str());
    }

    T clientUser;

    INFO("p4 command: " << command);
    for (const auto &arg : stringArguments) {
        INFO("arguments: " << arg);
    }

    m_ClientAPI.SetArgv(argsCharArray.size(), argsCharArray.data());
    m_ClientAPI.Run(command, &clientUser);

    int retries = commandRetries;
    while (m_ClientAPI.Dropped() || clientUser.GetError().IsError()) {
        if (retries == 0) {
            break;
        }

        ERROR("Connection dropped or command errored, retrying in 5 seconds.");
        std::this_thread::sleep_for(std::chrono::seconds(5));

        if (Reinitialize()) {
            INFO("Reinitialized P4API");
        } else {
            ERROR("Could not reinitialize P4API");
        }

        WARN("Retrying: p4 " << command << argsString);

        clientUser = T();

        m_ClientAPI.SetArgv(argsCharArray.size(), argsCharArray.data());
        m_ClientAPI.Run(command, &clientUser);

        retries--;
    }

    if (m_ClientAPI.Dropped() || clientUser.GetError().IsFatal()) {
        ERROR("Exiting due to receiving errors even after retrying " << COMMAND_RETRIES << " times");
        Deinitialize();
        std::exit(1);
    }

    m_Usage++;
    if (m_Usage > COMMAND_REFRESH_THRESHOLD) {
        int refreshRetries = COMMAND_RETRIES;
        while (refreshRetries > 0) {
            WARN("Trying to refresh the connection due to age (" << m_Usage << " > " << COMMAND_REFRESH_THRESHOLD << ").");
            if (Reinitialize()) {
                INFO("Connection was refreshed");
                break;
            }
            ERROR("Could not refresh connection due to old age. Retrying in 5 seconds");
            std::this_thread::sleep_for(std::chrono::seconds(5));

            refreshRetries--;
        }

        if (refreshRetries == 0) {
            ERROR("Could not refresh the connection after " << COMMAND_RETRIES << " retries. Exiting.");
            std::exit(1);
        }
    }

    return clientUser;
}
