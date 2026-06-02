#pragma once

#include "commands/result.h"
#include "reconcile/types.h"

#include <string>
#include <vector>

class P5;

class FstatCollector : public Result {
public:
    void OutputStat(StrDict *varList) override;

    const reconcile::DepotState &state() const { return m_state; }
    reconcile::DepotState &state() { return m_state; }

    static reconcile::DepotState Load(P5 &p5, const std::vector<std::string> &paths);

private:
    reconcile::DepotState m_state;
};
