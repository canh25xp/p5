#pragma once

#include "CLI/CLI.hpp"
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

    const std::string &port() const { return m_port; }
    const std::string &user() const { return m_user; }
    const std::string &client() const { return m_client; }
    bool resolveClient() const { return m_resolve_client; }
    bool autoLogin() const { return m_auto_login; }
    bool savePassword() const { return m_save_password; }
    const std::vector<std::pair<std::string, std::string>> &p4Protocol() const { return m_p4_protocol; }

private:
    std::unique_ptr<class Enviro> m_env;
    std::string m_user;
    std::string m_port;
    std::string m_client;
    bool m_resolve_client{true};
    bool m_auto_login{false};
    bool m_save_password{false};
    CLI::Option *m_resolve_client_option{nullptr};
    /// Each `-z` argument: `name` or `name=value` (Helix protocol variable).
    std::vector<std::string> m_protocol_z;
    /// Parsed `-z` entries (empty `value` when no `=` was present).
    std::vector<std::pair<std::string, std::string>> m_p4_protocol;
    std::string m_verbosity;
};

extern Options g_options;
