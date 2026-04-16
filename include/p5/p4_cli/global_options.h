#pragma once

#include "CLI/CLI.hpp"
#include <string>

namespace p4_cli {

/// Values bound to global `-u` / `-p` / `-c` (CLI + `envname`); filled during parse.
struct GlobalOptions {
    std::string user;
    std::string port;
    std::string client;
};

/// Registers global `-u` / `-p` / `-c` and `parse_complete_callback` to apply them to `P5` before subcommands run.
void register_global_options(CLI::App &app, GlobalOptions &opts);

} // namespace p4_cli
