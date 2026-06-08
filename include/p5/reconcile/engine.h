#pragma once

#include "p4api.h"
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

AnalyzeResult Analyze(P4API &p4api, const std::string &workDir, const std::vector<std::string> &paths, const ReconcileOptions &opts, p5::WorkspaceCache &cache);

void ApplyPlan(P4API &p4api, const ReconcilePlan &plan, const p5::DepotState &depot, const ReconcileOptions &opts);

} // namespace reconcile
