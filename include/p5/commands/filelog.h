#pragma once

#include "commands/command.h"
#include "types/filelog.h"

#include <string>
#include <vector>

class P4API;

class Filelog : public Command {
public:
    Filelog() : Command("filelog", "List revision history of files") {}

    void OutputStat(StrDict *varList) override;

    const std::vector<p5::FilelogFile> &files() const { return m_files; }

    static std::vector<p5::FilelogFile> Load(P4API &p4api, const std::vector<std::string> &args);

    void run(const std::vector<std::string> &args) override;
    void register_cli(CLI::App &app) override;

    std::vector<std::string> buildP4Args(const std::vector<std::string> &files) const;

private:
    std::vector<p5::FilelogFile> m_files;

    int m_changelist{0};
    bool m_contentHistory{false};
    bool m_inherited{false};
    bool m_long{false};
    bool m_longTrunc{false};
    bool m_time{false};
    int m_max{0};
    bool m_noPromote{false};
    bool m_short{false};
};
