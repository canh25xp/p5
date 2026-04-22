#include "registry.h"

#include "commands.h"
#include "command_runners.h"

void register_commands(CLI::App &app) {
    Commands commands;

    commands.add_passthrough("info", "Print out client/server information");
    commands.add_passthrough("sync", "Synchronize the client with its view of the depot");
    commands.add_passthrough("changes", "Display list of pending and submitted changelists");
    commands.add_passthrough("describe", "Display a changelist description");
    commands.add_passthrough("files", "List files in the depot");
    commands.add_passthrough("clients", "List users' clients (p4 clients)");
    commands.add("users", "List Perforce users (p5 formatted output)", run_users);

    commands.install(app);
    register_set(app);
}
