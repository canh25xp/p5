#pragma once

#include "p5.h"
#include "reconcile/types.h"

#include <string>
#include <vector>

namespace reconcile {

WorkspaceState ScanWorkspace(P5 &p5, const std::string &rootDir, bool skipIgnoreCheck);

} // namespace reconcile
