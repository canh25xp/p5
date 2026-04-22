#pragma once

#include <string>
#include <vector>

#include "CLI/CLI.hpp"

/// Batches CLI11 subcommand definitions; call install to attach them to a root `CLI::App`.
class Commands {
public:
    /// `prefix_command` subcommand: forwards `remaining()` to `run`.
    void add(const char *name, const char *description, void (*run)(const std::vector<std::string> &));

    /// Same as add, but invokes `run_p4_passthrough(p4_name, args)`; `p4_name` is also the subcommand name.
    void add_passthrough(const char *p4_name, const char *description);

    void install(CLI::App &app) const;

    void clear();

private:
    struct Entry {
        bool passthrough;
        const char *name;
        const char *description;
        void (*fn)(const std::vector<std::string> &);
    };
    std::vector<Entry> m_entries;
};
