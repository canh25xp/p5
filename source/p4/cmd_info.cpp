#include "CLI/CLI.hpp"

#include "p4/command_registrars.h"
#include "p4/run_forwarded.h"

namespace p4 {

void register_info(CLI::App &app) {
    auto *cmd = app.add_subcommand("info", "Print out client/server information");
    cmd->prefix_command();
    cmd->callback([cmd]() { run_p4_passthrough("info", cmd->remaining()); });
}

} // namespace p4
