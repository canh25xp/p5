#include "commands.h"

#include "run_forwarded.h"

void Commands::add(const char *name, const char *description, void (*run)(const std::vector<std::string> &), std::vector<const char *> aliases) {
    Entry e;
    e.passthrough = false;
    e.name = name;
    e.description = description;
    e.fn = run;
    e.aliases = std::move(aliases);
    m_entries.push_back(e);
}

void Commands::add_passthrough(const char *p4_name, const char *description, std::vector<const char *> aliases) {
    Entry e;
    e.passthrough = true;
    e.name = p4_name;
    e.description = description;
    e.fn = nullptr;
    e.aliases = std::move(aliases);
    m_entries.push_back(e);
}

void Commands::clear() {
    m_entries.clear();
}

void Commands::install(CLI::App &app) const {
    for (const auto &e : m_entries) {
        CLI::App *sub = app.add_subcommand(e.name, e.description);
        sub->prefix_command();

        // Register aliases so the subcommand can be invoked by any of its alternative names.
        for (const char *alias : e.aliases) {
            sub->alias(alias);
        }

        if (e.passthrough) {
            const char *p4 = e.name;
            sub->callback([p4, sub]() { run_p4_passthrough(p4, sub->remaining()); });
        } else {
            void (*fn)(const std::vector<std::string> &) = e.fn;
            sub->callback([fn, sub]() { fn(sub->remaining()); });
        }
    }
}
