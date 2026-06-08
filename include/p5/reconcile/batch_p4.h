#pragma once

#include "p4api.h"

#include <functional>
#include <string>
#include <vector>

namespace reconcile {

/// Run a P4 command with batched file arguments (respects Windows argv length limits).
void RunBatched(P4API &p4api, const std::vector<std::string> &alwaysArgs, const std::vector<std::string> &fileArgs, bool useChangelist, int changelist);

} // namespace reconcile
