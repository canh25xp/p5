#pragma once

#include "p5.h"
#include "types/workspace.h"

#include <string>
#include <vector>

namespace reconcile {

p5::WorkspaceState ScanWorkspace(P5 &p5, const std::string &rootDir, bool skipIgnoreCheck);

} // namespace reconcile
