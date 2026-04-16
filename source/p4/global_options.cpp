#include "p4/global_options.h"

#include "p4/run_forwarded.h"

namespace p4 {

void register_global_options(CLI::App &app, GlobalOptions &opts) {
    app.usage("[GLOBAL OPTIONS...] <subcommand> [arguments...]");

    app.add_option("-u,--user", opts.user, "Override P4USER (default: $P4USER)")
        ->group("Global options")
        ->envname("P4USER");
    app.add_option("-p,--port", opts.port, "Override P4PORT (default: $P4PORT)")
        ->group("Global options")
        ->envname("P4PORT");
    app.add_option("-c,--client", opts.client, "Override P4CLIENT (default: $P4CLIENT)")
        ->group("Global options")
        ->envname("P4CLIENT");

    // Runs after argv and env merge, before any subcommand callback (CLI11 root `run_callback` order).
    app.parse_complete_callback(
        [&opts]() { apply_connection_options(opts.user, opts.port, opts.client); });
}

} // namespace p4
