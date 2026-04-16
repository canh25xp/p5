#include "CLI/CLI.hpp"

#include "p4/command_registrars.h"
#include "p4/run_forwarded.h"

namespace p4 {

void register_sync(CLI::App &app) {
    auto *cmd = app.add_subcommand("sync", "Synchronize the client with its view of the depot");
    cmd->prefix_command();
    cmd->callback([cmd]() { run_p4_passthrough("sync", cmd->remaining()); });
}

} // namespace p4
