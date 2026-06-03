#pragma once

#include "p5.h"

#include <functional>
#include <string>
#include <vector>

namespace reconcile {

/// Run a P4 command with batched file arguments (respects Windows argv length limits).
void RunBatched(P5 &p5, const std::vector<std::string> &alwaysArgs, const std::vector<std::string> &fileArgs, bool useChangelist, int changelist);

} // namespace reconcile
