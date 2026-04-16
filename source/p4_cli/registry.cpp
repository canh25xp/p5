#include "p4_cli/registry.h"

#include "p4_cli/command_registrars.h"

namespace p4_cli {

void register_commands(CLI::App &app) {
    register_info(app);
    register_sync(app);
    register_changes(app);
    register_describe(app);
    register_files(app);
}

} // namespace p4_cli
