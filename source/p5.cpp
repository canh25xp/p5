#include "p5.h"

#include <csignal>
#include <cstdlib>
#include <string>
#include <vector>
#include <chrono>
#include <thread>

#include "CLI/CLI.hpp"
#include "p4/clientapi.h"
#include "p4/p4libs.h"
#include "p4/signaler.h"

#include "command_runners.h"
#include "log.h"
#include "cli_helpers.h"

std::string P5::P4PORT;
std::string P5::P4USER;
std::string P5::P4CLIENT;
std::vector<std::pair<std::string, std::string>> P5::P4PROTOCOL_Z;

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

P5::P5(P5ForCliConfig) : m_Usage(0), m_LibrariesInitialized(false) {}

void P5::register_cli(CLI::App &app) {
    register_global_options(app, m_globalOptions);
    m_commands.clear();

    // --- File operations ---
    m_commands.add_passthrough("add", "Open a new file to add it to the depot");
    m_commands.add_passthrough("delete", "Open an existing file to delete it from the depot");
    m_commands.add_passthrough("edit", "Open an existing file for edit");
    m_commands.add_passthrough("move", "Moves files from one location to another", {"rename"});
    m_commands.add_passthrough("reopen", "Change the type or changelist number of an opened file");
    m_commands.add_passthrough("revert", "Discard changes from an opened file");
    m_commands.add_passthrough("lock", "Lock an opened file against changelist submission");
    m_commands.add_passthrough("unlock", "Release a locked file but leave it open");

    // --- Changelist operations ---
    m_commands.add_passthrough("change", "Create or edit a changelist description", {"changelist"});
    m_commands.add_passthrough("changes", "Display list of pending and submitted changelists", {"changelists"});
    m_commands.add_passthrough("describe", "Display a changelist description");
    m_commands.add_passthrough("shelve", "Store files from a pending changelist into the depot");
    m_commands.add_passthrough("unshelve", "Restore shelved files from a pending changelist");
    m_commands.add_passthrough("reshelve", "Copy shelved files to a new or existing shelf");
    m_commands.add_passthrough("submit", "Submit open files to the depot");
    m_commands.add_passthrough("resolve", "Merge open files with other revisions or files");
    m_commands.add_passthrough("resolved", "Show files that have been merged but not submitted");

    // --- Sync / workspace state ---
    m_commands.add_passthrough("sync", "Synchronize the client with its view of the depot");
    m_commands.add_passthrough("update", "Update the client with its view of the depot");
    m_commands.add_passthrough("flush", "Fake a 'p4 sync' by not moving files");
    m_commands.add_passthrough("clean", "Delete or refresh local files to match depot state");
    m_commands.add_passthrough("reconcile", "Reconcile client to offline workspace changes", {"rec"});
    m_commands.add_passthrough("status", "Preview reconcile of client to offline workspace changes");
    m_commands.add_passthrough("have", "List revisions last synced");
    m_commands.add_passthrough("opened", "Display list of files opened for pending changelist");
    m_commands.add_passthrough("where", "Show how file names map through the client view");
    m_commands.add_passthrough("ignores", "List P4IGNORE mappings");

    // --- Client / workspace ---
    m_commands.add_passthrough("client", "Create or edit a client specification and its view", {"workspace"});
    m_commands.add_passthrough("clients", "Display list of known clients", {"workspaces"});

    // --- Depot / file info ---
    m_commands.add_passthrough("files", "List files in the depot");
    m_commands.add_passthrough("fstat", "Dump file info");
    m_commands.add_passthrough("filelog", "List revision history of files");
    m_commands.add_passthrough("annotate", "Print file lines along with their revisions");
    m_commands.add_passthrough("diff", "Display diff of client file with depot file");
    m_commands.add_passthrough("diff2", "Display diff of two depot files");
    m_commands.add_passthrough("print", "Retrieve a depot file to the standard output");
    m_commands.add_passthrough("dirs", "List subdirectories of a given depot directory");
    m_commands.add_passthrough("sizes", "Display size information for files in the depot");
    m_commands.add_passthrough("grep", "Print lines from text files matching a pattern");
    m_commands.add_passthrough("cstat", "Dump change/sync status for current client");

    // --- Integration ---
    m_commands.add_passthrough("integrate", "Schedule integration from one file to another");
    m_commands.add_passthrough("integrated", "Show integrations that have been submitted");
    m_commands.add_passthrough("interchanges", "Report changes that have not yet been integrated");
    m_commands.add_passthrough("merge", "Schedule merge (integration) from one file to another");
    m_commands.add_passthrough("copy", "Schedule copy of latest rev from one file to another");
    m_commands.add_passthrough("undo", "Undo a range of revisions");

    // --- Branch / label / tag ---
    m_commands.add_passthrough("branch", "Create or edit a branch specification");
    m_commands.add_passthrough("branches", "Display list of branches");
    m_commands.add_passthrough("label", "Create or edit a label specification and its view");
    m_commands.add_passthrough("labels", "Display list of labels");
    m_commands.add_passthrough("labelsync", "Synchronize label with the current client contents");
    m_commands.add_passthrough("list", "Create an in-memory (label) list of depot files");
    m_commands.add_passthrough("tag", "Tag files with a label");

    // --- Stream ---
    m_commands.add_passthrough("stream", "Create or edit a stream specification");
    m_commands.add_passthrough("streams", "Display list of streams");
    m_commands.add_passthrough("streamlog", "List revision history of streams");
    m_commands.add_passthrough("streamspec", "Edit the stream template");
    m_commands.add_passthrough("switch", "Switch to a different stream, or create a new stream");
    m_commands.add_passthrough("istat", "Show integrations needed for a stream");
    m_commands.add_passthrough("populate", "Populate a branch or stream with files");
    m_commands.add_passthrough("prune", "Remove unmodified branched files from a stream");

    // --- Job ---
    m_commands.add_passthrough("job", "Create or edit a job (defect) specification");
    m_commands.add_passthrough("jobs", "Display list of jobs");
    m_commands.add_passthrough("fix", "Mark jobs as being fixed by named changelists");
    m_commands.add_passthrough("fixes", "List what changelists fix what job");

    // --- User / group ---
    m_commands.add("users", "List Perforce users (p5 formatted output)", run_users);
    m_commands.add_passthrough("user", "Create or edit a user specification");
    m_commands.add_passthrough("group", "Change members of a user group");
    m_commands.add_passthrough("groups", "List groups (of users)");
    m_commands.add_passthrough("passwd", "Set the user's password on the server (and Windows client)");

    // --- Depot / repo admin ---
    m_commands.add_passthrough("depot", "Create or edit a depot specification");
    m_commands.add_passthrough("depots", "Display list of depots");
    m_commands.add_passthrough("repo", "Create, edit or delete a repo specification");
    m_commands.add_passthrough("repos", "Display a list of repos (depots of type graph)");

    // --- Counter / key ---
    m_commands.add_passthrough("counter", "Display, set, or delete a counter");
    m_commands.add_passthrough("counters", "Display list of known counters");
    m_commands.add_passthrough("key", "Display, set, or delete a key/value pair");
    m_commands.add_passthrough("keys", "Display list of known keys and their values");

    // --- Protection / review ---
    m_commands.add_passthrough("protect", "Modify protections in the server namespace");
    m_commands.add_passthrough("protects", "Display protections in place for a given user/path");
    m_commands.add_passthrough("review", "List and track changelists (for the review daemon)");
    m_commands.add_passthrough("reviews", "Show what users are subscribed to review files");
    m_commands.add_passthrough("attribute", "Set per-revision attributes on revisions");

    // --- Auth ---
    m_commands.add_passthrough("login", "Login to Perforce by obtaining a session ticket");
    m_commands.add_passthrough("login2", "Perform a multi factor authentication");
    m_commands.add_passthrough("logout", "Logout of Perforce by removing or invalidating a ticket");
    m_commands.add_passthrough("tickets", "Display list of session tickets for this user");
    m_commands.add_passthrough("trust", "Establish trust of an SSL connection");

    // --- Info / logging ---
    m_commands.add_passthrough("info", "Print out client/server information");
    m_commands.add_passthrough("logger", "Report what jobs and changelists have changed");
    m_commands.add_passthrough("aliases", "Display the content of the P4ALIASES file");
    m_commands.add_passthrough("help", "Print the requested help message");

    m_commands.install(app);
    register_set(app);
}

bool P5::Initialize() {
    Error e;
    StrBuf msg;

    m_Usage = 0;
    m_ClientAPI.SetPort(P4PORT.c_str());
    m_ClientAPI.SetUser(P4USER.c_str());
    m_ClientAPI.SetClient(P4CLIENT.c_str());
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

P5::LibrariesGuard::LibrariesGuard() : m_initialized(false) {
    m_initialized = InitializeLibraries();
}

P5::LibrariesGuard::~LibrariesGuard() {
    if (m_initialized) {
        ShutdownLibraries();
    }
}

bool P5::LibrariesGuard::initialized() const {
    return m_initialized;
}

Users P5::ListUsers(const std::vector<std::string> &extraArgs) {
    std::vector<std::string> args;
    args.reserve(1 + extraArgs.size());
    args.push_back("-a"); // Include service accounts
    args.insert(args.end(), extraArgs.begin(), extraArgs.end());
    return Run<Users>("users", args);
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
