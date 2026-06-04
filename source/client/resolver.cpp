#include "client/resolver.h"

#include <iostream>

std::string Resolver::resolve(const MergeInfo &info) {
    if (info.mergeHint == "e") {
        std::cerr << "Standard resolver encountered merge conflict, skipping resolve\n";
        return "s";
    }
    return info.mergeHint;
}

std::string Resolver::actionResolve(const MergeInfo &info) {
    return info.mergeHint;
}
