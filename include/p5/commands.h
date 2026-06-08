#pragma once

#include <memory>

#include <CLI/CLI.hpp>
#include "commands/command.h"

class P5;

// Batches CLI11 subcommand definitions; call install to attach them to a root `CLI::App`.
class Commands {
public:
    Commands() {};
    ~Commands() {};

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

    // Get or create the P5 instance (lazy initialization).
    // Returns a reference to the shared P5 instance.
    P5 &p5();

    // Passthrough to P5::Run - uses owned P5 instance.
    void run_p4_passthrough(const char *command, const std::vector<std::string> &args);

private:
    std::vector<std::unique_ptr<Command>> m_entries;
    std::unique_ptr<P5> m_p5; // Owned P5 instance for connection reuse
};
