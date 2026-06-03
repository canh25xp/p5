#pragma once

#include "commands/command.h"

#include <string>

class Mirror : public Command {
public:
    Mirror() : Command("mirror", "Create a client mirroring another client's view and sync history") {}

    void run(const std::vector<std::string> &args) override;
    void register_cli(CLI::App &app) override;

private:
    std::string m_templateClient;
    std::string m_mirrorClient;
};
