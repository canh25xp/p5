#include "CLI/CLI.hpp"

#include "p4/command_registrars.h"
#include "p4/run_forwarded.h"

namespace p4 {

void register_describe(CLI::App &app) {
    auto *cmd = app.add_subcommand("describe", "Display a changelist description");
    cmd->prefix_command();
    cmd->callback([cmd]() { run_p4_passthrough("describe", cmd->remaining()); });
}

} // namespace p4
