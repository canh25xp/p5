#pragma once

#include "command.h"

#include <string>
#include <vector>

class Changes : public Command {
public:
    Changes() : Command("changes", "Display list of pending and submitted changelists", {"changelists"}) {}

    void run(const std::vector<std::string> &args) override;
    void register_cli(CLI::App &app) override;

    std::vector<std::string> buildP4Args(const std::vector<std::string> &files) const;

private:
    bool m_integrated{false};
    bool m_time{false};
    bool m_long{false};
    bool m_longTrunc{false};
    bool m_force{false};
    bool m_reverse{false};
    std::vector<std::string> m_client;
    bool m_clientCaseInsensitive{false};
    int m_afterChange{0};
    int m_max{0};
    std::string m_status;
    std::vector<std::string> m_user;
    bool m_userCaseInsensitive{false};
    bool m_stream{false};
    bool m_nostream{false};
};
