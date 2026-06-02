#pragma once

#include "commands/result.h"
#include "reconcile/types.h"

#include <string>
#include <vector>

class P5;

namespace reconcile {

class FstatCollector : public Result {
public:
    void OutputStat(StrDict *varList) override;

    const DepotState &state() const { return m_state; }
    DepotState &state() { return m_state; }

    static DepotState Load(P5 &p5, const std::vector<std::string> &paths);

private:
    DepotState m_state;
};

} // namespace reconcile
