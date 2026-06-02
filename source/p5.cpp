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
#include "utils/command_policy.h"
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
    for (const auto &proto : g_options.protocol()) {
        m_ClientAPI.SetProtocol(proto.first.c_str(), proto.second.c_str());
    }

    m_ClientAPI.Init(&e);

    if (g_options.client().empty() && g_options.resolve() && CommandPolicy::NeedsClientResolve(g_options.command())) {
        std::string resolved = AutoResolve();
        if (!resolved.empty()) {
            g_options.client() = resolved;
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

    // Fetch clients owned by the current user (isolated API so tag protocol does not leak)
    Clients clientsResult = FetchClientsTagged({"--me"});
    if (clientsResult.IsError()) {
        WARN("Auto-resolve: could not fetch clients list");
        return {};
    }

    // Filter to clients on the current host
    Clients::ClientMap filtered = ClientResolver::FilterByHost(clientsResult.GetClients(), hostname);

    if (filtered.empty()) {
        INFO("Auto-resolve: no clients found for user " << g_options.user() << " on host " << hostname);
        return {};
    }

    // Resolve the best-matching client based on CWD
    std::string resolved = ClientResolver::Resolve(cwd, filtered);
    if (resolved.empty()) {
        INFO("Auto-resolve: no client root matches CWD " << cwd);
        return {};
    }

    INFO("Auto-resolve: matched client " << resolved << " for CWD " << cwd);

    const char *configPathCstr = m_ClientAPI.GetConfig().Text();
    std::string configPath = configPathCstr ? configPathCstr : "";
    ClientResolver::WriteClientToConfig(configPath, resolved);

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

ClientUser &P5::Execute(const std::string &command, const std::vector<std::string> &args, ClientUser &user) {
    std::vector<char *> argv;
    argv.reserve(args.size());
    for (const auto &arg : args) {
        argv.push_back(const_cast<char *>(arg.c_str()));
    }
    m_ClientAPI.SetArgv(static_cast<int>(argv.size()), argv.empty() ? nullptr : argv.data());
    m_ClientAPI.Run(command.c_str(), &user);
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
    Result clientUser;
    Execute(command.c_str(), args, clientUser);
    return clientUser;
}

void P5::SetTagProtocol() {
    m_ClientAPI.SetProtocol("tag", "");
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
T P5::Run(const std::string &command, const std::vector<std::string> &args, const int commandRetries) {
    std::string argsString;
    for (const std::string &stringArg : args) {
        argsString = argsString + " " + stringArg;
    }

    T clientUser;

    INFO("Run: p4 " << command << argsString);

    Execute(command, args, clientUser);

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

        Execute(command, args, clientUser);

        retries--;
    }

    if (m_ClientAPI.Dropped() || clientUser.IsFatal()) {
        ERROR("Exiting due to receiving errors even after retrying " << COMMAND_RETRIES << " times");
        Deinitialize();
        std::exit(1);
    }

    RefreshIfNeeded();

    return clientUser;
}

// Explicit instantiations
template Users P5::Run<Users>(const std::string &command, const std::vector<std::string> &stringArguments, const int commandRetries);
template Clients P5::Run<Clients>(const std::string &command, const std::vector<std::string> &stringArguments, const int commandRetries);
template Changes P5::Run<Changes>(const std::string &command, const std::vector<std::string> &stringArguments, const int commandRetries);
template Fstat P5::Run<Fstat>(const std::string &command, const std::vector<std::string> &stringArguments, const int commandRetries);
template Have P5::Run<Have>(const std::string &command, const std::vector<std::string> &stringArguments, const int commandRetries);

Clients P5::FetchClientsTagged(const std::vector<std::string> &args) {
    ClientApi api;
    Error e;
    StrBuf msg;

    api.SetPort(g_options.port().c_str());
    api.SetUser(g_options.user().c_str());
    api.SetProtocol("tag", "");
    api.Init(&e);

    if (e.Test()) {
        e.Fmt(&msg);
        ERROR(msg.Text());
        return Clients();
    }

    std::vector<char *> argv;
    argv.reserve(args.size());
    for (const auto &arg : args) {
        argv.push_back(const_cast<char *>(arg.c_str()));
    }
    api.SetArgv(static_cast<int>(argv.size()), argv.empty() ? nullptr : argv.data());

    std::string argsString;
    for (const std::string &stringArg : args) {
        argsString = argsString + " " + stringArg;
    }
    INFO("FetchClientsTagged: p4 clients" << argsString);

    Clients clients;
    api.Run("clients", &clients);

    api.Final(&e);
    CheckErrors(e, msg);

    return clients;
}

Clients P5::RunClients(const std::vector<std::string> &args) {
    m_ClientAPI.SetProtocol("tag", "");
    return Run<Clients>("clients", args);
}

Users P5::RunUsers(const std::vector<std::string> &args) {
    m_ClientAPI.SetProtocol("tag", "");
    return Run<Users>("users", args);
}

Changes P5::RunChanges(const std::vector<std::string> &args) {
    m_ClientAPI.SetProtocol("tag", "");
    return Run<Changes>("changes", args);
}

Fstat P5::RunFstat(const std::vector<std::string> &args) {
    m_ClientAPI.SetProtocol("tag", "");
    return Run<Fstat>("fstat", args);
}

Have P5::RunHave(const std::vector<std::string> &args) {
    m_ClientAPI.SetProtocol("tag", "");
    return Run<Have>("have", args);
}
