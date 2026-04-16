#include "p4/registry.h"

#include "p4/command_registrars.h"

namespace p4 {

void register_commands(CLI::App &app) {
    register_info(app);
    register_sync(app);
    register_changes(app);
    register_describe(app);
    register_files(app);
}

} // namespace p4
