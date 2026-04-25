#include "commands.h"
#include "p5.h"
#include "log.h"

void Commands::add(const char *name, const char *description, void (*run)(const std::vector<std::string> &), std::vector<const char *> aliases) {
    std::vector<std::string> aliases_str;
    for (const char *alias : aliases) {
        aliases_str.push_back(alias);
    }
    m_entries.push_back(std::make_unique<Command>(name, description, std::move(aliases_str), false, run));
}

void Commands::add(std::unique_ptr<Command> cmd) {
    m_entries.push_back(std::move(cmd));
}

void Commands::add(const char *name, const char *description, std::vector<const char *> aliases) {
    std::vector<std::string> aliases_str;
    for (const char *alias : aliases) {
        aliases_str.push_back(alias);
    }
    m_entries.push_back(std::make_unique<Command>(name, description, std::move(aliases_str), true, nullptr));
}

void Commands::add(std::vector<const char *> commands, const char *description) {
    const char *name = commands[0];
    std::vector<std::string> aliases;
    for (size_t i = 1; i < commands.size(); ++i) {
        aliases.push_back(commands[i]);
    }
    m_entries.push_back(std::make_unique<Command>(name, description, std::move(aliases), true, nullptr));
}

void Commands::clear() {
    m_entries.clear();
}

void Commands::install(CLI::App &app) const {
    for (const auto &e : m_entries) {
        e->register_cli(app);
    }
}

void Commands::run_p4_passthrough(const char *command, const std::vector<std::string> &args) {
    P5 p5;

    INFO("p4 command passthrough: " << command);
    for (const auto &arg : args) {
        INFO("arguments: " << arg);
    }

    std::vector<char *> argv_array;
    argv_array.reserve(args.size());
    for (const auto &arg : args) {
        argv_array.push_back(const_cast<char *>(arg.c_str()));
    }

    char **argv_ptr = argv_array.empty() ? nullptr : argv_array.data();
    p5.Run(command, static_cast<int>(args.size()), argv_ptr);
}

void Command::run(const std::vector<std::string> &args) {
    if (passthrough) {
        Commands::run_p4_passthrough(name.c_str(), args);
    } else if (fn) {
        fn(args);
    }
}

void Command::register_cli(CLI::App &app) {
    CLI::App *sub = app.add_subcommand(name, description);
    sub->prefix_command();

    // Register aliases so the subcommand can be invoked by any of its alternative names.
    for (const std::string &alias : aliases) {
        sub->alias(alias);
    }

    sub->callback([this, sub]() { this->run(sub->remaining()); });
}
