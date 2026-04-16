#include "CLI/CLI.hpp"

#include "p4_cli/command_registrars.h"
#include "p4_cli/run_forwarded.h"

namespace p4_cli {

void register_sync(CLI::App &app) {
    auto *cmd = app.add_subcommand("sync", "Synchronize the client with its view of the depot");
    cmd->prefix_command();
    cmd->callback([cmd]() { run_p4_passthrough("sync", cmd->remaining()); });
}

} // namespace p4_cli
