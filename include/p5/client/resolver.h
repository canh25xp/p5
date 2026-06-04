#pragma once

#include <string>

// Carries the merge hint from a P4API resolve callback.
// The hint indicates the auto-resolve result:
// "m" merged, "e" edit/conflict, "t" theirs, "y" yours, "s" skip, "q" quit.
struct MergeInfo {
    std::string mergeHint;
};

// Abstract base for handling file-merge and action resolve callbacks during sync/submit.
// Override resolve() for file merge resolution and actionResolve() for action resolution.
// The default implementation skips conflicts and accepts auto-resolved results.
class Resolver {
public:
    virtual ~Resolver() = default;

    virtual std::string resolve(const MergeInfo &info);
    virtual std::string actionResolve(const MergeInfo &info);
};
