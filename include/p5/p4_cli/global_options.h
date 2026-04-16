#pragma once

#include "CLI/CLI.hpp"
#include <string>

namespace p4_cli {

/// Registers global `-u` / `-p` / `-c` and `parse_complete_callback` to apply them to `P5` before subcommands run.
void register_global_options(CLI::App &app, std::string &user, std::string &port, std::string &client);

} // namespace p4_cli
