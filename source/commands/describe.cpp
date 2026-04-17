#include "CLI/CLI.hpp"

#include "command_registrars.h"
#include "run_forwarded.h"

void register_describe(CLI::App &app) {
    auto *cmd = app.add_subcommand("describe", "Display a changelist description");
    cmd->prefix_command();
    cmd->callback([cmd]() { run_p4_passthrough("describe", cmd->remaining()); });
}
