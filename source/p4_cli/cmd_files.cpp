#include "CLI/CLI.hpp"

#include "p4_cli/command_registrars.h"
#include "p4_cli/run_forwarded.h"

namespace p4_cli {

void register_files(CLI::App &app) {
    auto *cmd = app.add_subcommand("files", "List files in the depot");
    cmd->prefix_command();
    cmd->callback([cmd]() { run_p4_passthrough("files", cmd->remaining()); });
}

} // namespace p4_cli
