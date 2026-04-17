#include "CLI/CLI.hpp"

#include "command_registrars.h"
#include "run_forwarded.h"

void register_files(CLI::App &app) {
    auto *cmd = app.add_subcommand("files", "List files in the depot");
    cmd->prefix_command();
    cmd->callback([cmd]() { run_p4_passthrough("files", cmd->remaining()); });
}
