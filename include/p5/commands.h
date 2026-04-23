#pragma once

#include <memory>

#include "CLI/CLI.hpp"
#include "commands/command.h"

/// Batches CLI11 subcommand definitions; call install to attach them to a root `CLI::App`.
class Commands {
public:
    /// `prefix_command` subcommand: forwards `remaining()` to `run`.
    void add(const char *name, const char *description, void (*run)(const std::vector<std::string> &), std::vector<const char *> aliases = {});

    void add(std::unique_ptr<Command> cmd);

    /// Passthrough overload: invokes `run_p4_passthrough(name, args)` instead of a custom callback.
    void add(const char *name, const char *description, std::vector<const char *> aliases = {});

    void install(CLI::App &app) const;

    void clear();

    static void run_p4_passthrough(const char *command, const std::vector<std::string> &args);

private:
    std::vector<std::unique_ptr<Command>> m_entries;
};
