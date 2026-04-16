#include "CLI/CLI.hpp"

#include "p4_cli/command_registrars.h"
#include "p4_cli/run_forwarded.h"

namespace p4_cli {

void register_changes(CLI::App &app) {
    auto *cmd = app.add_subcommand("changes", "Display list of pending and submitted changelists");
    cmd->prefix_command();
    cmd->callback([cmd]() { run_p4_passthrough("changes", cmd->remaining()); });
}

} // namespace p4_cli
