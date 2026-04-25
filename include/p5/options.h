#pragma once

#include "CLI/CLI.hpp"
#include <string>
#include <utility>
#include <vector>

/// Batches global CLI11 flags; call `add` to attach to a root `CLI::App`, then `apply` after parse
/// (invoked from `parse_complete_callback`). `apply` finalizes values for the Helix client connection.
class Options {
public:
    void add(CLI::App &app);
    void apply();
    void load_enviro(class Enviro &env) const;

    const std::string &port() const { return m_port; }
    const std::string &user() const { return m_user; }
    const std::string &client() const { return m_client; }
    bool resolveClient() const { return m_resolve_client; }
    const std::vector<std::pair<std::string, std::string>> &p4Protocol() const { return m_p4_protocol; }

private:
    // TODO: these options should align with p4's Enviro (When run `set`)
    // Maybe also refactor `source/commands/set.cpp` to ensure a single source of truth for environtment.
    // See home/michael/projects/p5-dev/vendor/p4api/p4api-2025.1.2907437/include/p4/enviro.h
    std::string m_user;
    std::string m_port;
    std::string m_client;
    bool m_resolve_client{true};
    /// Each `-z` argument: `name` or `name=value` (Helix protocol variable).
    std::vector<std::string> m_protocol_z;
    /// Parsed `-z` entries (empty `value` when no `=` was present).
    std::vector<std::pair<std::string, std::string>> m_p4_protocol;

    CLI::Option *m_user_opt{nullptr};
    CLI::Option *m_port_opt{nullptr};
    CLI::Option *m_client_opt{nullptr};
};

extern Options g_options;
