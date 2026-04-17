#include "CLI/CLI.hpp"

#include "command_registrars.h"
#include "run_forwarded.h"

namespace p4 {

void register_clients(CLI::App &app) {
    auto *cmd = app.add_subcommand("clients", "List users' clients (p4 clients)");
    cmd->prefix_command();
    cmd->callback([cmd]() { run_p4_passthrough("clients", cmd->remaining()); });
}

} // namespace p4
