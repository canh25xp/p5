#pragma once

#include "commands/command.h"
#include "types/filelog.h"

#include <vector>

class P5;

class Filelog : public Command {
public:
    Filelog() : Command("filelog", "List revision history of files") {}

    void OutputStat(StrDict *varList) override;

    const std::vector<p5::FilelogFile> &files() const { return m_files; }

    static std::vector<p5::FilelogFile> Load(P5 &p5, const std::vector<std::string> &args);

    void run(const std::vector<std::string> &args) override;
    void register_cli(CLI::App &app) override;

private:
    std::vector<p5::FilelogFile> m_files;
};
