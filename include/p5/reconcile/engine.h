#pragma once

#include "p5.h"
#include "reconcile/types.h"

#include <string>
#include <vector>

namespace reconcile {

struct AnalyzeResult {
    ReconcilePlan plan;
    DepotState depot;
};

AnalyzeResult Analyze(P5 &p5, const std::string &workDir, const std::vector<std::string> &paths, const ReconcileOptions &opts, WorkspaceCache &cache);

void ApplyPlan(P5 &p5, const ReconcilePlan &plan, const DepotState &depot, const ReconcileOptions &opts);

} // namespace reconcile
