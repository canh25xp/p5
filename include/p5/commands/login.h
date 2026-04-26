#pragma once

#include "command.h"

class Login : public Command {
    bool m_save{false};

public:
    Login() : Command("login", "Login to Perforce by obtaining a session ticket") {}

    void run(const std::vector<std::string> &args) override;
    void register_cli(CLI::App &app) override;
};
