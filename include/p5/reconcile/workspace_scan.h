#pragma once

#include "p4api.h"
#include "types/workspace.h"

#include <string>
#include <vector>

namespace reconcile {

p5::WorkspaceState ScanWorkspace(P4API &p4api, const std::string &rootDir, bool skipIgnoreCheck);

} // namespace reconcile
