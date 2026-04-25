#include "p5.h"

#include <csignal>
#include <cstdlib>
#include <string>
#include <vector>
#include <chrono>
#include <thread>

#include "p4/clientapi.h"
#include "p4/p4libs.h"
#include "p4/signaler.h"

#include "log.h"
#include "options.h"
#include "cli_helpers.h"
#include "client_resolver.h"

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
    // Auto-resolve client based on CWD if no explicit client was provided
    // Guard against re-entry: AutoResolveClient -> ListClients -> Reinitialize -> Initialize
    if (g_options.client().empty() && g_options.resolveClient() && !m_AutoResolving) {
        AutoResolveClient();
    }

    m_ClientAPI.Init(&e);

    if (!CheckErrors(e, msg)) {
        ERROR("Could not initialize Helix Core C/C++ API");
        return false;
    }

    return true;
}

void P5::AutoResolveClient() {
    m_AutoResolving = true;

    std::string cwd = ClientResolver::GetCurrentWorkingDirectory();
    if (cwd.empty()) {
        WARN("Auto-resolve: could not determine current working directory");
        m_AutoResolving = false;
        return;
    }

    std::string hostname = ClientResolver::GetCurrentHostname();

    // Fetch clients owned by the current user using a transient API
    // to avoid polluting the main m_ClientAPI with "tag" protocol.
    ClientApi bootstrapApi;
    bootstrapApi.SetPort(g_options.port().c_str());
    bootstrapApi.SetUser(g_options.user().c_str());
    bootstrapApi.SetProtocol("tag", "");

    Error e;
    bootstrapApi.Init(&e);
    if (e.Test()) {
        StrBuf msg;
        e.Fmt(&msg);
        WARN("Auto-resolve: could not initialize bootstrap connection: " << msg.Text());
        m_AutoResolving = false;
        return;
    }

    Clients clientsResult;
    const char *args[] = {"--me"};
    bootstrapApi.SetArgv(1, const_cast<char **>(args));
    bootstrapApi.Run("clients", &clientsResult);

    bootstrapApi.Final(&e);

    // Filter to clients on the current host
    Clients::ClientMap filtered = ClientResolver::FilterByHost(
        clientsResult.GetClients(), hostname);

    if (filtered.empty()) {
        INFO("Auto-resolve: no clients found for user on host " << hostname);
        m_AutoResolving = false;
        return;
    }

    // Resolve the best-matching client based on CWD
    std::string resolved = ClientResolver::Resolve(cwd, filtered);
    if (!resolved.empty()) {
        INFO("Auto-resolve: matched client " << resolved << " for CWD " << cwd);
        m_ClientAPI.SetClient(resolved.c_str());
    } else {
        INFO("Auto-resolve: no client root matches CWD " << cwd);
    }

    m_AutoResolving = false;
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

    // We disable the default signaler to stop it from deleting memory from the wrong heap
    // https://www.perforce.com/manuals/p4api/Content/P4API/chapter.clientprogramming.signaler.html
    std::signal(SIGINT, SIG_DFL);
    signaler.Disable();

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

Users P5::ListUsers(const std::vector<std::string> &extraArgs) {
    // Use tag protocol by default so OutputStat is called with structured data
    m_ClientAPI.SetProtocol("tag", "");

    std::vector<std::string> args;
    args.reserve(1 + extraArgs.size());
    args.push_back("-a"); // Include service accounts
    args.insert(args.end(), extraArgs.begin(), extraArgs.end());
    return Run<Users>("users", args);
}

Clients P5::ListClients(const std::vector<std::string> &extraArgs) {
    // Use tag protocol by default so OutputStat is called with structured data
    m_ClientAPI.SetProtocol("tag", "");

    std::vector<std::string> args;
    args.reserve(extraArgs.size());
    args.insert(args.end(), extraArgs.begin(), extraArgs.end());
    return Run<Clients>("clients", args);
}

Result P5::Run(const char *command, int argumentCount, char **arguments) {
    Result clientUser;

    m_ClientAPI.SetArgv(argumentCount, arguments);
    m_ClientAPI.Run(command, &clientUser);

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

    // Build argv (must stay alive during Run)
    std::vector<const char *> argv;

    argv.reserve(args.size());

    for (std::string &arg : args) {
        argv.push_back(arg.c_str()); // safe: std::string owns memory
    }

    return Run(command.c_str(), static_cast<int>(argv.size()), const_cast<char **>(argv.data()));
}

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
