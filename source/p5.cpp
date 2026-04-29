#include "p5.h"

#include <cstdlib>
#include <string>
#include <vector>
#include <chrono>
#include <thread>

#include <p4/clientapi.h>
#include <p4/p4libs.h>

#include "log.h"
#include "options.h"
#include "utils/cli_helpers.h"
#include "utils/client_resolver.h"

P5::P5() : m_Usage(0), m_LibrariesInitialized(false) {
    if (!InitializeLibraries()) {
        ERROR("Could not initialize P4 libraries");
        return;
    }
    m_LibrariesInitialized = true;
    if (!Initialize()) {
        ERROR("Could not initialize P4");
        return;
    }
}

bool P5::Initialize() {
    Error e;
    StrBuf msg;

    m_Usage = 0;
    m_ClientAPI.SetPort(g_options.port().c_str());
    m_ClientAPI.SetUser(g_options.user().c_str());
    m_ClientAPI.SetClient(g_options.client().c_str());
    for (const auto &proto : g_options.p4Protocol()) {
        m_ClientAPI.SetProtocol(proto.first.c_str(), proto.second.c_str());
    }

    m_ClientAPI.Init(&e);

    if (g_options.client().empty() && g_options.resolveClient()) {
        std::string resolved = AutoResolve();
        if (!resolved.empty()) {
            m_ClientAPI.SetClient(resolved.c_str());
        }
    }

    if (!CheckErrors(e, msg)) {
        ERROR("Could not initialize Helix Core C/C++ API");
        return false;
    }

    return true;
}

std::string P5::AutoResolve() {
    std::string cwd = ClientResolver::GetCurrentWorkingDirectory();
    if (cwd.empty()) {
        WARN("Auto-resolve: could not determine current working directory");
    }

    std::string hostname = ClientResolver::GetCurrentHostname();

    // Fetch clients owned by the current user
    Clients clientsResult = ListClients({"--me"});

    // Filter to clients on the current host
    Clients::ClientMap filtered = ClientResolver::FilterByHost(clientsResult.GetClients(), hostname);

    if (filtered.empty()) {
        INFO("Auto-resolve: no clients found for user " << g_options.user() << " on host " << hostname);
        return {};
    }

    // Resolve the best-matching client based on CWD
    std::string resolved = ClientResolver::Resolve(cwd, filtered);
    if (resolved.empty())
        INFO("Auto-resolve: no client root matches CWD " << cwd);

    INFO("Auto-resolve: matched client " << resolved << " for CWD " << cwd);
    return resolved;
}

bool P5::Deinitialize() {
    Error e;
    StrBuf msg;

    m_ClientAPI.Final(&e);
    CheckErrors(e, msg);

    return true;
}

bool P5::Reinitialize() {
    bool status = Deinitialize() && Initialize();
    return status;
}

P5::~P5() {
    if (!Deinitialize()) {
        ERROR("P4 context was not destroyed successfully");
    }
    if (m_LibrariesInitialized && !ShutdownLibraries()) {
        ERROR("P4 libraries were not shut down successfully");
    }
}

bool P5::CheckErrors(Error &e, StrBuf &msg) {
    if (e.Test()) {
        e.Fmt(&msg);
        ERROR(msg.Text());
        return false;
    }
    return true;
}

bool P5::InitializeLibraries() {
    Error e;
    StrBuf msg;
    P4Libraries::Initialize(P4LIBRARIES_INIT_ALL, &e);
    if (e.Test()) {
        e.Fmt(&msg);
        ERROR(msg.Text());
        ERROR("Failed to initialize P4Libraries");
        return false;
    }

    INFO("Initialized P4Libraries successfully");
    return true;
}

bool P5::ShutdownLibraries() {
    Error e;
    StrBuf msg;
    P4Libraries::Shutdown(P4LIBRARIES_INIT_ALL, &e);
    if (e.Test()) {
        e.Fmt(&msg);
        ERROR(msg.Text());
        return false;
    }

    INFO("Shutdown Initialized P4Libraries successfully");
    return true;
}

ClientUser &P5::Execute(const char *command, std::vector<char *> &argv, ClientUser &user) {
    m_ClientAPI.SetArgv(static_cast<int>(argv.size()), argv.empty() ? nullptr : argv.data());
    m_ClientAPI.Run(command, &user);
    return user;
}

void P5::RefreshIfNeeded() {
    m_Usage++;
    if (m_Usage <= COMMAND_REFRESH_THRESHOLD) {
        return;
    }

    int refreshRetries = COMMAND_RETRIES;
    while (refreshRetries-- > 0) {
        WARN("Trying to refresh the connection due to age (" << m_Usage << " > " << COMMAND_REFRESH_THRESHOLD << ").");
        if (Reinitialize()) {
            INFO("Connection was refreshed");
            m_Usage = 0;
            return;
        }

        ERROR("Could not refresh connection due to old age. Retrying in 5 seconds");
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }

    ERROR("Could not refresh the connection after " << COMMAND_RETRIES << " retries. Exiting.");
    std::exit(1);
}

Result P5::Run(const std::string &command, const std::vector<std::string> &args) {
    std::vector<char *> argv;
    argv.reserve(args.size());
    for (const auto &arg : args) {
        argv.push_back(const_cast<char *>(arg.c_str()));
    }

    Result clientUser;
    Execute(command.c_str(), argv, clientUser);

    return clientUser;
}

Result P5::Run(const std::string &commandLine) {
    // Split string into tokens
    std::vector<std::string> tokens = SplitCommandLine(commandLine);

    if (tokens.empty()) {
        return Result();
    }

    // First token is the p4 command
    const std::string &command = tokens[0];

    // Remaining tokens are arguments
    std::vector<std::string> args(tokens.begin() + 1, tokens.end());

    return Run(command, args);
}

template <class T>
T P5::Run(const char *command, const std::vector<std::string> &stringArguments, const int commandRetries) {
    std::string argsString;
    for (const std::string &stringArg : stringArguments) {
        argsString = argsString + " " + stringArg;
    }

    std::vector<char *> argsCharArray;
    for (const std::string &arg : stringArguments) {
        argsCharArray.push_back((char *)arg.c_str());
    }

    T clientUser;

    INFO("Run: p4 " << command << argsString);

    Execute(command, argsCharArray, clientUser);

    int retries = commandRetries;
    while (m_ClientAPI.Dropped() || clientUser.IsError()) {
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

        Execute(command, argsCharArray, clientUser);

        retries--;
    }

    if (m_ClientAPI.Dropped() || clientUser.GetError().IsFatal()) {
        ERROR("Exiting due to receiving errors even after retrying " << COMMAND_RETRIES << " times");
        Deinitialize();
        std::exit(1);
    }

    RefreshIfNeeded();

    return clientUser;
}

// Explicit instantiations
template Users P5::Run<Users>(const char *command, const std::vector<std::string> &stringArguments, const int commandRetries);
template Clients P5::Run<Clients>(const char *command, const std::vector<std::string> &stringArguments, const int commandRetries);
