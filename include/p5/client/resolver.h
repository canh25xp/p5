#pragma once

#include <string>

/// Merge metadata for resolve callbacks (P4Python mergeInfo).
struct MergeInfo {
    std::string mergeHint;
};

/// P4Python Resolver — override for automatic/content resolve.
class Resolver {
public:
    virtual ~Resolver() = default;

    virtual std::string resolve(const MergeInfo &info);
    virtual std::string actionResolve(const MergeInfo &info);
};
