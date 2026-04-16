#include "p4_cli/global_options.h"

#include "p4_cli/run_forwarded.h"

namespace p4_cli {

void register_global_options(CLI::App &app, std::string &user, std::string &port, std::string &client) {
    app.usage("[GLOBAL OPTIONS...] <subcommand> [arguments...]");

    app.add_option("-u,--user", user, "Override P4USER (default: $P4USER)")
        ->group("Global options")
        ->envname("P4USER");
    app.add_option("-p,--port", port, "Override P4PORT (default: $P4PORT)")
        ->group("Global options")
        ->envname("P4PORT");
    app.add_option("-c,--client", client, "Override P4CLIENT (default: $P4CLIENT)")
        ->group("Global options")
        ->envname("P4CLIENT");

    // Runs after argv and env merge, before any subcommand callback (CLI11 root `run_callback` order).
    app.parse_complete_callback([&]() { apply_connection_options(user, port, client); });
}

} // namespace p4_cli
