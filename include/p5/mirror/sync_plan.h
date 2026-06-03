#pragma once

#include <string>
#include <vector>

namespace mirror {

enum class SyncStepKind {
    Cumulative, // sync ...@CL
    Exact,      // sync ...@=CL
};

struct SyncStep {
    SyncStepKind kind;
    int changelist;
};

/// Build an optimized sequence of sync steps to replicate a template client's #have changelist history.
/// Input must be sorted ascending with no duplicates.
std::vector<SyncStep> BuildSyncPlan(const std::vector<int> &sortedChangelists);

/// Format a sync file argument revision suffix (e.g. "@7" or "@=10").
std::string FormatSyncRevision(const SyncStep &step);

} // namespace mirror
