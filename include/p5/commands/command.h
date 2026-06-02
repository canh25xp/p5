#pragma once

#include <string>
#include <vector>
#include "result.h"

namespace CLI {
class App;
}

class Commands; // Forward declaration

/// Represents a single command in the p5 application.
class Command : public Result {
public:
    Command(std::string name, std::string description, std::vector<std::string> aliases = {}, bool passthrough = false, void (*fn)(const std::vector<std::string> &) = nullptr)
        : name(std::move(name)), description(std::move(description)), aliases(std::move(aliases)), passthrough(passthrough), fn(fn) {}

    virtual ~Command() = default;

    /// Called during register_cli to store Commands reference for P5 access
    void set_commands(Commands *commands) { m_commands = commands; }

    virtual void run(const std::vector<std::string> &args);
    virtual void register_cli(CLI::App &app);

    std::string name;
    std::vector<std::string> aliases;
    std::string description;

    bool passthrough;
    void (*fn)(const std::vector<std::string> &);

protected:
    Commands *m_commands = nullptr; ///< For accessing shared P5 instance
};
