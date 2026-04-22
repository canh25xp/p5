#pragma once

#include "CLI/CLI.hpp"
#include <string>
#include <vector>

/// Batches global CLI11 flags; call `add` to attach to a root `CLI::App`, then `apply` after parse
/// (invoked from `parse_complete_callback`). `apply` copies values into `P5` statics.
class Options {
public:
    void add(CLI::App &app);
    void apply();

private:
    std::string m_user;
    std::string m_port;
    std::string m_client;
    /// Each `-z` argument: `name` or `name=value` (Helix protocol variable).
    std::vector<std::string> m_protocol_z;
};
