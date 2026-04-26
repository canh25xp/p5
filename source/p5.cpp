#include "p5.h"

#include <csignal>
#include <cstdlib>
#include <string>
#include <vector>
#include <chrono>
#include <fstream>
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

