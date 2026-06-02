#pragma once

#include "command.h"

class FastReconcile : public Command {
public:
    FastReconcile() : Command("fast-reconcile", "Fast offline reconcile (same as rec, optimized)", {"fast-rec"}) {}

    void run(const std::vector<std::string> &args) override;
    void register_cli(CLI::App &app) override;

private:
    bool m_preview{false};
    bool m_edit{false};
    bool m_add{false};
    bool m_delete{false};
    bool m_mtime{false};
    bool m_skipIgnore{false};
    int m_changelist{0};
    bool m_flagEdit{false};
    bool m_flagAdd{false};
    bool m_flagDelete{false};

    static bool NeedsPassthrough(const std::vector<std::string> &args);
};
