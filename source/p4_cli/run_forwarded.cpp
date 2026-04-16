#include "p4_cli/run_forwarded.h"

#include <vector>

#include "CLI/CLI.hpp"
#include "dbg.h"

#include "log.h"
#include "p5.h"

namespace p4_cli {

void apply_connection_options(const std::string &user, const std::string &port, const std::string &client) {
    P5::P4USER = user;
    P5::P4PORT = port;
    P5::P4CLIENT = client;
}

void run_forwarded(P5 &p5, const char *command, const std::vector<std::string> &args) {
    std::vector<char *> argv_array;
    argv_array.reserve(args.size());
    for (const auto &arg : args) {
        argv_array.push_back(const_cast<char *>(arg.c_str()));
    }

    char **argv_ptr = argv_array.empty() ? nullptr : argv_array.data();
    p5.Run(command, static_cast<int>(args.size()), argv_ptr);
}

void run_p4_passthrough(const char *command, const std::vector<std::string> &args) {
    if (!P5::InitializeLibraries()) {
        throw CLI::RuntimeError(1);
    }

    bool ok = P5().TestConnection().IsError();
    if (ok) {
        INFO("Perforce server is available");
    } else {
        ERROR("Error occurred while connecting to " << P5::P4PORT);
        throw CLI::RuntimeError(1);
    }

    P5 p5;

    dbg(command);
    for (const auto &arg : args) {
        dbg(arg);
    }

    run_forwarded(p5, command, args);

    if (!P5::ShutdownLibraries()) {
        throw CLI::RuntimeError(1);
    }
}

} // namespace p4_cli
