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
#include "cli_helpers.h"

std::string P5::P4PORT;
std::string P5::P4USER;
std::string P5::P4CLIENT;
std::vector<std::pair<std::string, std::string>> P5::P4PROTOCOL_Z;

P5::P5() {
    if (!Initialize()) {
        ERROR("Could not initialize P4");
        return;
    }
}

bool P5::Initialize() {
    Error e;
    StrBuf msg;

    m_Usage = 0;
    m_ClientAPI.SetPort(P4PORT.c_str());
    m_ClientAPI.SetUser(P4USER.c_str());
    m_ClientAPI.SetClient(P4CLIENT.c_str());
    m_ClientAPI.SetProtocol("tag", "");
    for (const auto &proto : P4PROTOCOL_Z) {
        m_ClientAPI.SetProtocol(proto.first.c_str(), proto.second.c_str());
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

TestResult P5::TestConnection(const int retries) {
    return Run<TestResult>("changes", {"-m", "1", "//..."}, retries);
}

UsersResult P5::Users(const std::vector<std::string> &extraArgs) {
    std::vector<std::string> args;
    args.reserve(1 + extraArgs.size());
    args.push_back("-a"); // Include service accounts
    args.insert(args.end(), extraArgs.begin(), extraArgs.end());
    return Run<p4::UsersResult>("users", args);
}

InfoResult P5::Info() {
    return Run<InfoResult>("info", {});
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
