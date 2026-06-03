#include "mirror/sync_plan.h"

namespace mirror {

std::vector<SyncStep> BuildSyncPlan(const std::vector<int> &sortedChangelists) {
    if (sortedChangelists.empty()) {
        return {};
    }

    std::vector<SyncStep> steps;
    size_t index = 0;

    // Initial consecutive run from the first changelist.
    size_t runEnd = 0;
    while (runEnd + 1 < sortedChangelists.size() && sortedChangelists[runEnd + 1] == sortedChangelists[runEnd] + 1) {
        ++runEnd;
    }

    if (sortedChangelists[0] == 1) {
        steps.push_back({SyncStepKind::Cumulative, sortedChangelists[runEnd]});
        index = runEnd + 1;
    } else {
        for (size_t i = 0; i <= runEnd; ++i) {
            steps.push_back({SyncStepKind::Exact, sortedChangelists[i]});
        }
        index = runEnd + 1;
    }

    // Remaining changelists after the first gap always use exact @= syncs.
    for (; index < sortedChangelists.size(); ++index) {
        steps.push_back({SyncStepKind::Exact, sortedChangelists[index]});
    }

    return steps;
}

std::string FormatSyncRevision(const SyncStep &step) {
    if (step.kind == SyncStepKind::Cumulative) {
        return "@" + std::to_string(step.changelist);
    }
    return "@=" + std::to_string(step.changelist);
}

} // namespace mirror
