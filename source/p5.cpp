#include "p5.h"

#include <csignal>
#include <cstdlib>
#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include <mutex>

#include "p4/clientapi.h"
#include "p4/p4libs.h"
#include "p4/signaler.h"

#include "log.h"
#include "std_helpers.h"
#include "stream_result.h"
#include "cli_helpers.h"

ClientResult::ClientSpecData P5::ClientSpec;
std::string P5::P4PORT;
std::string P5::P4USER;
std::string P5::P4CLIENT;
std::mutex P5::InitializationMutex;

P5::P5() {
    if (!Initialize()) {
        ERROR("Could not initialize P4");
        return;
    }

    AddClientSpecView(ClientSpec.mapping);
}

bool P5::Initialize() {
    // Helix Core C++ API seems to crash while making connections parallely.
    std::unique_lock<std::mutex> lock(InitializationMutex);

    Error e;
    StrBuf msg;

    m_Usage = 0;
    m_ClientAPI.SetPort(P4PORT.c_str());
    m_ClientAPI.SetUser(P4USER.c_str());
    m_ClientAPI.SetClient(P4CLIENT.c_str());
    m_ClientAPI.SetProtocol("tag", "");
    m_ClientAPI.Init(&e);

    if (!CheckErrors(e, msg)) {
        ERROR("Could not initialize Helix Core C/C++ API");
        return false;
    }

    return true;
}

bool P5::Deinitialize() {
    std::unique_lock<std::mutex> lock(InitializationMutex);

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

bool P5::IsDepotPathValid(const std::string &depotPath) {
    return STDHelpers::EndsWith(depotPath, "/...") && STDHelpers::StartsWith(depotPath, "//");
}

bool P5::IsFileUnderDepotPath(const std::string &fileRevision, const std::string &depotPath) {
    return STDHelpers::Contains(fileRevision, depotPath.substr(0, depotPath.size() - 3)); // -3 to remove the trailing "..."
}

bool P5::IsDepotPathUnderClientSpec(const std::string &depotPath) {
    return m_ClientMapping.IsInLeft(depotPath);
}

bool P5::IsFileUnderClientSpec(const std::string &fileRevision) {
    return m_ClientMapping.IsInRight(fileRevision);
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

void P5::AddClientSpecView(const std::vector<std::string> &viewStrings) {
    m_ClientMapping.InsertTranslationMapping(viewStrings);
}

void P5::UpdateClientSpec() {
    Run<Result>("client", {});
}

ClientResult P5::Client() {
    return Run<ClientResult>("client", {"-o"});
}

StreamResult P5::Stream(const std::string &path) {
    return Run<StreamResult>("stream", {"-o", path});
}

TestResult P5::TestConnection(const int retries) {
    return Run<TestResult>("changes", {"-m", "1", "//..."}, retries);
}

ChangesResult P5::ShortChanges(const std::string &path) {
    ChangesResult changes = Run<ChangesResult>("changes", {
                                                              "-s", "submitted", // Only include submitted CLs
                                                              path               // Depot path to get CLs from
                                                          });
    changes.reverse();
    return changes;
}

ChangesResult P5::Changes(const std::string &path) {
    return Run<ChangesResult>("changes", {
                                             "-l",              // Get full descriptions instead of sending cut-short ones
                                             "-s", "submitted", // Only include submitted CLs
                                             path               // Depot path to get CLs from
                                         });
}

ChangesResult P5::Changes(const std::string &path, const std::string &from, int32_t maxCount) {
    std::vector<std::string> args = {
        "-l",             // Get full descriptions instead of sending cut-short ones
        "-s", "submitted" // Only include submitted CLs
    };

    // This needs to be declared outside the if scope below to
    // keep the internal character array alive till the p4 call is made
    std::string maxCountStr;
    if (maxCount != -1) {
        maxCountStr = std::to_string(maxCount);

        args.push_back("-m"); // Only send max this many number of CLs
        args.push_back(maxCountStr);
    }

    std::string pathAddition;
    if (!from.empty()) {
        // Appending "@CL_NUMBER,@now" seems to include the current CL,
        // which makes this awkward to deal with in general. So instead,
        // we append "@>CL_NUMBER" so that we only receive the CLs after
        // the current one.
        pathAddition = "@>" + from;
    }

    args.push_back(path + pathAddition);

    ChangesResult result = Run<ChangesResult>("changes", args);
    result.reverse();
    return result;
}

ChangesResult P5::ChangesFromTo(const std::string &path, const std::string &from, const std::string &to) {
    std::string pathArg = path + "@" + from + "," + to;
    return Run<ChangesResult>("changes", {
                                             "-s", "submitted", // Only include submitted CLs
                                             pathArg            // Depot path to get CLs from
                                         });
}

ChangesResult P5::LatestChange(const std::string &path) {
    return Run<ChangesResult>("changes", {
                                             "-s", "submitted", // Only include submitted CLs,
                                             "-m", "1",         // Get top-most change
                                             path               // Depot path to get CLs from
                                         });
}

ChangesResult P5::OldestChange(const std::string &path) {
    ChangesResult changes = Run<ChangesResult>("changes", {
                                                              "-s", "submitted", // Only include submitted CLs,
                                                              "-m", "1",         // Get top-most change
                                                              path               // Depot path to get CLs from
                                                          });
    changes.reverse();
    return changes;
}

DescribeResult P5::Describe(const std::string &cl) {
    return Run<DescribeResult>("describe", {"-s", // Omit the diffs
                                            cl});
}

FileLogResult P5::FileLog(const std::string &changelist) {
    return Run<FileLogResult>("filelog", {
                                             "-c", // restrict output to a single changelist
                                             changelist,
                                             "-m1",  // don't get the full history, just the first entry.
                                             "//..." // rather than require the path to be passed in, just list all files.
                                         });
}

SizesResult P5::Size(const std::string &file) {
    return Run<SizesResult>("sizes", {"-a", "-s", file});
}

Result P5::Sync() {
    return Run<Result>("sync", {});
}

SyncResult P5::GetFilesToSyncAtCL(const std::string &path, const std::string &cl) {
    std::string clCommand = "@" + cl;
    return Run<SyncResult>("sync", {
                                       "-n", // Only preview the files to sync. Don't send file contents...yet
                                       clCommand,
                                   });
}

PrintResult P5::PrintFile(const std::string &filePathRevision) {
    return Run<PrintResult>("print", {
                                         filePathRevision,
                                     });
}

PrintResult P5::PrintFiles(const std::vector<std::string> &fileRevisions) {
    if (fileRevisions.empty()) {
        return PrintResult();
    }

    return Run<PrintResult>("print", fileRevisions);
}

Result P5::Sync(const std::string &path) {
    return Run<Result>("sync", {
                                   path // Sync a particular depot path
                               });
}

UsersResult P5::Users(const std::vector<std::string> &extraArgs) {
    std::vector<std::string> args;
    args.reserve(1 + extraArgs.size());
    args.push_back("-a"); // Include service accounts
    args.insert(args.end(), extraArgs.begin(), extraArgs.end());
    return Run<UsersResult>("users", args);
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
