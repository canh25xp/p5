#include "registry.h"

#include "command_registrars.h"

void register_commands(CLI::App &app) {
    register_info(app);
    register_sync(app);
    register_changes(app);
    register_describe(app);
    register_files(app);
    register_clients(app);
    register_users(app);
    register_set(app);
}
