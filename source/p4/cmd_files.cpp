#include "CLI/CLI.hpp"

#include "p4/command_registrars.h"
#include "p4/run_forwarded.h"

namespace p4 {

void register_files(CLI::App &app) {
    auto *cmd = app.add_subcommand("files", "List files in the depot");
    cmd->prefix_command();
    cmd->callback([cmd]() { run_p4_passthrough("files", cmd->remaining()); });
}

} // namespace p4
