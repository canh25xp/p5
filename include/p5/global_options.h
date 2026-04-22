#pragma once

#include "CLI/CLI.hpp"
#include <string>
#include <vector>

/// Values bound to global `-u` / `-p` / `-c` (CLI + `envname`); filled during parse.
struct GlobalOptions {
    std::string user;
    std::string port;
    std::string client;
    /// Each `-z` argument: `name` or `name=value` (Helix protocol variable).
    std::vector<std::string> protocol_z;
};

/// Registers global `-u` / `-p` / `-c` / `-z` and `parse_complete_callback` to copy values into `P5` statics before subcommands run.
/// Used by `P5::register_cli`.
void register_global_options(CLI::App &app, GlobalOptions &opts);
