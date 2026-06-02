#pragma once

#include "commands/result.h"
#include "reconcile/types.h"

#include <string>
#include <unordered_map>
#include <vector>

class P5;

namespace reconcile {

class HaveCollector : public Result {
public:
    void OutputStat(StrDict *varList) override;

    const std::unordered_map<std::string, HaveRecord> &records() const { return m_records; }

    static std::unordered_map<std::string, HaveRecord> Load(P5 &p5, const std::vector<std::string> &paths);

private:
    std::unordered_map<std::string, HaveRecord> m_records;
};

} // namespace reconcile
