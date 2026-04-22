#pragma once

#include "command.h"

class Options;

class Set : public Command {
    const Options &m_options;
    bool m_all{false};
    bool m_quiet{false};

public:
    Set(const Options &options)
        : Command("set", "Set or display Perforce variables"), m_options(options) {}

    void run(const std::vector<std::string> &args) override;
    void register_cli(CLI::App &app) override;
};
