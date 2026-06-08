#pragma once

#include <memory>

#include <CLI/CLI.hpp>
#include "commands/command.h"

class P4API;

// Batches CLI11 subcommand definitions; call install to attach them to a root `CLI::App`.
class Commands {
public:
    Commands();
    ~Commands();

    // `prefix_command` subcommand: forwards `remaining()` to `run`.
    void add(const char *name, const char *description, void (*run)(const std::vector<std::string> &), std::vector<const char *> aliases = {});

    void add(std::unique_ptr<Command> cmd);

    // Passthrough overload: invokes `run_p4_passthrough(name, args)` instead of a custom callback.
    void add(const char *name, const char *description, std::vector<const char *> aliases = {});

    // First command is the main command, the rest is aliases.
    // Example: add({"client", "workspace"}, "Create or edit a client specification")
    void add(std::vector<const char *> commands, const char *description);

    void install(CLI::App &app) const;

    void clear();

    // Get or create the P4API instance (lazy initialization).
    // Returns a reference to the shared P4API instance.
    P4API &p4api();

    // Passthrough to P4API::Run - uses owned P5 instance.
    void run_p4_passthrough(const char *command, const std::vector<std::string> &args);

private:
    std::vector<std::unique_ptr<Command>> m_entries;
    std::unique_ptr<P4API> m_p4api; // Owned P4API instance for connection reuse
};
