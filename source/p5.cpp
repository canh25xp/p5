#include "p5.h"

#include <csignal>
#include <cstdlib>
#include <openssl/crypto.h>
#include <string>
#include <vector>
#include <chrono>
#include <thread>

#include <p4/clientapi.h>
#include <p4/p4libs.h>
#include <p4/signaler.h>

#include "log.h"
#include "options.h"
#include "utils/cli_helpers.h"
#include "utils/client_resolver.h"
#include "utils/auth_store.h"

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
    if (g_options.client().empty() && g_options.resolveClient()) {
        std::string resolved = ClientResolver::AutoResolve(g_options.port(), g_options.user());
        if (!resolved.empty()) {
            m_ClientAPI.SetClient(resolved.c_str());
        }
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

void P5::Run(const char *command, const std::vector<std::string> &args, ClientUser &clientUser) {
    std::vector<char *> argv;
    argv.reserve(args.size());
    for (const std::string &arg : args) {
        argv.push_back(const_cast<char *>(arg.c_str()));
    }

    m_ClientAPI.SetArgv(static_cast<int>(argv.size()), argv.empty() ? nullptr : argv.data());
    m_ClientAPI.Run(command, &clientUser);
}

namespace {

class SilentResult : public ClientUser {
public:
    bool HasError() const { return m_error.Test() != 0; }
    bool IsFatal() const { return m_error.IsFatal() != 0; }

    void HandleError(Error *e) override { m_error = *e; }
    void Message(Error *e) override {
        if (e && e->Test()) {
            m_error = *e;
        }
    }
    void OutputError(const char *) override {}
    void OutputInfo(char, const char *) override {}
    void OutputText(const char *, int) override {}

private:
    Error m_error;
};

} // namespace

bool P5::AutoLogin() {
    char statusArg[] = "-s";
    char *statusArgs[] = {statusArg};
    SilentResult status;
    m_ClientAPI.SetArgv(1, statusArgs);
    m_ClientAPI.Run("login", &status);
    if (!status.HasError()) {
        return true;
    }

    std::string password;
    if (!AuthStore::LoadPassword(Port(), User(), password)) {
        return false;
    }
    if (password.empty()) {
        return false;
    }

    m_ClientAPI.SetPassword(password.c_str());

    SilentResult login;
    m_ClientAPI.SetArgv(0, nullptr);
    m_ClientAPI.Run("login", &login);
    OPENSSL_cleanse(&password[0], password.size());
    password.clear();
    return !login.HasError() && !login.IsFatal();
}

std::string P5::Port() {
    return m_ClientAPI.GetPort().Text();
}

std::string P5::User() {
    return m_ClientAPI.GetUser().Text();
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

// Explicit instantiations
template Users P5::Run<Users>(const char *command, const std::vector<std::string> &stringArguments, const int commandRetries);
template Clients P5::Run<Clients>(const char *command, const std::vector<std::string> &stringArguments, const int commandRetries);
