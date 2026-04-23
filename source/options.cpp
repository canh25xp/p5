#include "options.h"

Options g_options;

#include <utility>

#ifndef P5_VERSION_STRING
#define P5_VERSION_STRING "p5 (unknown version) (p4api unknown)"
#endif

void Options::add(CLI::App &app) {
    app.usage("[GLOBAL OPTIONS...] <subcommand> [arguments...]");

    app.set_version_flag("-v,-V,--version", P5_VERSION_STRING)->group("Global options");

    app.add_option("-u,--user", m_user, "Override P4USER (default: $P4USER)")
        ->group("Global options")
        ->envname("P4USER");
    app.add_option("-p,--port", m_port, "Override P4PORT (default: $P4PORT)")
        ->group("Global options")
        ->envname("P4PORT");
    app.add_option("-c,--client", m_client, "Override P4CLIENT (default: $P4CLIENT)")
        ->group("Global options")
        ->envname("P4CLIENT");
    app.add_flag("--no-auto-client", m_no_auto_client, "Disable automatic client resolution based on CWD")
        ->group("Global options");
    app.add_option("-z", m_protocol_z, "Set Helix protocol variable (name or name=value); repeat for multiple")
        ->group("Global options");

    // Runs after argv and env merge, before any subcommand callback (CLI11 root `run_callback` order).
    app.parse_complete_callback([this]() { apply(); });
}

void Options::apply() {
    m_p4_protocol.clear();
    m_p4_protocol.reserve(m_protocol_z.size());
    for (const std::string &entry : m_protocol_z) {
        const std::string::size_type eq = entry.find('=');
        std::string key = eq == std::string::npos ? entry : entry.substr(0, eq);
        std::string value = eq == std::string::npos ? std::string() : entry.substr(eq + 1);
        if (!key.empty()) {
            m_p4_protocol.emplace_back(std::move(key), std::move(value));
        }
    }
}
