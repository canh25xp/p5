#pragma once

#include <vector>
#include <string>

// Split commands string in to tokens
// Input "p4 changes -m1 depot/path/..."
// Output { "p4", "changes", "-m1", "depot/path/..." }
std::vector<std::string> SplitCommandLine(const std::string &input);
