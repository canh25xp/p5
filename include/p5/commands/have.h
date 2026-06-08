#pragma once

#include "commands/command.h"
#include "types/have.h"

#include <string>
#include <unordered_map>
#include <vector>

class P4API;

class Have : public Command {
public:
    Have() : Command("have", "List files the client workspace has synced") {}

    void OutputStat(StrDict *varList) override;

    const std::unordered_map<std::string, p5::HaveRecord> &records() const { return m_records; }

    static std::unordered_map<std::string, p5::HaveRecord> Load(P4API &p4api, const std::vector<std::string> &paths);

    void run(const std::vector<std::string> &args) override;
    void register_cli(CLI::App &app) override;

private:
    std::unordered_map<std::string, p5::HaveRecord> m_records;
};
