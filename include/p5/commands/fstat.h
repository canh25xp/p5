#pragma once

#include "commands/command.h"
#include "types.h"

#include <string>
#include <vector>

class P5;

class Fstat : public Command {
public:
    Fstat() : Command("fstat", "Display file information") {}

    void OutputStat(StrDict *varList) override;

    const p5::DepotState &state() const { return m_state; }
    p5::DepotState &state() { return m_state; }

    static p5::DepotState Load(P5 &p5, const std::vector<std::string> &paths);

    void run(const std::vector<std::string> &args) override;
    void register_cli(CLI::App &app) override;

private:
    p5::DepotState m_state;
};
