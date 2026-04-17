#include "CLI/CLI.hpp"

#include "command_registrars.h"
#include "p5.h"

#include <iostream>

namespace p4 {

static void run_p5_users(const std::vector<std::string> &args) {
    if (!P5::InitializeLibraries()) {
        throw CLI::RuntimeError(1);
    }

    bool printDone = false;
    {
        P5 p5;
        UsersResult r = p5.Users(args);
        // Result::IsError() returns true when there is no client-side error.
        if (r.IsError()) {
            r.PrintSortedTsv(std::cout);
            printDone = true;
        }
    }

    if (!P5::ShutdownLibraries()) {
        throw CLI::RuntimeError(1);
    }

    if (!printDone) {
        throw CLI::RuntimeError(1);
    }
}

void register_users(CLI::App &app) {
    auto *cmd = app.add_subcommand("users", "List Perforce users (p5 formatted output)");
    cmd->prefix_command();
    cmd->callback([cmd]() { run_p5_users(cmd->remaining()); });
}

} // namespace p4
