#pragma once

#include "p5.h"
#include "types/depot.h"
#include "types/reconcile.h"
#include "types/workspace.h"

#include <string>
#include <vector>

namespace reconcile {

struct AnalyzeResult {
    ReconcilePlan plan;
    p5::DepotState depot;
};

AnalyzeResult Analyze(P5 &p5, const std::string &workDir, const std::vector<std::string> &paths, const ReconcileOptions &opts, p5::WorkspaceCache &cache);

void ApplyPlan(P5 &p5, const ReconcilePlan &plan, const p5::DepotState &depot, const ReconcileOptions &opts);

} // namespace reconcile
