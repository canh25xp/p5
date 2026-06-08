#pragma once

#include "commands/command.h"
#include "types/depot.h"

#include <string>
#include <vector>

class P4API;

class Fstat : public Command {
public:
    Fstat() : Command("fstat", "Display file information") {}

    void OutputStat(StrDict *varList) override;

    const p5::DepotState &state() const { return m_state; }
    p5::DepotState &state() { return m_state; }

    static p5::DepotState Load(P4API &p4api, const std::vector<std::string> &paths);

    void run(const std::vector<std::string> &args) override;
    void register_cli(CLI::App &app) override;

private:
    p5::DepotState m_state;
};
