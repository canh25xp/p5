#pragma once

#include "commands/command.h"

class Map : public Command {
public:
    Map() : Command("map", "View and edit client workspace view mappings") {}

    void run(const std::vector<std::string> &args) override;
    void register_cli(CLI::App &app) override;
};
