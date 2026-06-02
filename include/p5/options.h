#pragma once

#include <CLI/CLI.hpp>
#include <memory>
#include <string>
#include <utility>
#include <vector>

/// Batches global CLI11 flags; call `add` to attach to a root `CLI::App`, then `apply` after parse
/// (invoked from `parse_complete_callback`). `apply` finalizes values for the Helix client connection.
class Options {
public:
    void add(CLI::App &app);
    void apply();
    class Enviro *env() const { return m_env.get(); }
    void load_enviro(class Enviro &env) const;

    std::string &port() { return m_port; }
    std::string &user() { return m_user; }
    std::string &client() { return m_client; }
    bool &resolve() { return m_resolve_client; }
    std::vector<std::pair<std::string, std::string>> &protocol() { return m_p4_protocol; }
    void set_command(const std::string &command) { m_command = command; }
    const std::string &port() const { return m_port; }
    const std::string &user() const { return m_user; }
    const std::string &client() const { return m_client; }
    bool resolve() const { return m_resolve_client; }
    const std::string &command() const { return m_command; }
    const std::vector<std::pair<std::string, std::string>> &protocol() const { return m_p4_protocol; }

private:
    std::unique_ptr<class Enviro> m_env;
    std::string m_user;
    std::string m_port;
    std::string m_client;
    std::string m_command;
    bool m_resolve_client{true};
    /// Each `-z` argument: `name` or `name=value` (Helix protocol variable).
    std::vector<std::string> m_protocol_z;
    /// Parsed `-z` entries (empty `value` when no `=` was present).
    std::vector<std::pair<std::string, std::string>> m_p4_protocol;
    std::string m_verbosity;
};

extern Options g_options;
