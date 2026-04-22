#include "commands.h"
#include "p5.h"
#include "dbg.h"

void Commands::add(const char *name, const char *description, void (*run)(const std::vector<std::string> &), std::vector<const char *> aliases) {
    std::vector<std::string> aliases_str;
    for (const char *alias : aliases) {
        aliases_str.push_back(alias);
    }
    m_entries.push_back(std::make_unique<Command>(name, description, std::move(aliases_str), false, run));
}

void Commands::add_passthrough(const char *p4_name, const char *description, std::vector<const char *> aliases) {
    std::vector<std::string> aliases_str;
    for (const char *alias : aliases) {
        aliases_str.push_back(alias);
    }
    m_entries.push_back(std::make_unique<Command>(p4_name, description, std::move(aliases_str), true, nullptr));
}

void Commands::clear() {
    m_entries.clear();
}

void Commands::install(CLI::App &app) const {
    for (const auto &e : m_entries) {
        CLI::App *sub = app.add_subcommand(e->name, e->description);
        sub->prefix_command();

        // Register aliases so the subcommand can be invoked by any of its alternative names.
        for (const std::string &alias : e->aliases) {
            sub->alias(alias);
        }

        if (e->passthrough) {
            std::string p4 = e->name;
            sub->callback([p4, sub]() { Commands::run_p4_passthrough(p4.c_str(), sub->remaining()); });
        } else {
            void (*fn)(const std::vector<std::string> &) = e->fn;
            sub->callback([fn, sub]() { fn(sub->remaining()); });
        }
    }
}

void Commands::run_p4_passthrough(const char *command, const std::vector<std::string> &args) {
    P5 p5;

    dbg(command);
    for (const auto &arg : args) {
        dbg(arg);
    }

    std::vector<char *> argv_array;
    argv_array.reserve(args.size());
    for (const auto &arg : args) {
        argv_array.push_back(const_cast<char *>(arg.c_str()));
    }

    char **argv_ptr = argv_array.empty() ? nullptr : argv_array.data();
    p5.Run(command, static_cast<int>(args.size()), argv_ptr);
}
