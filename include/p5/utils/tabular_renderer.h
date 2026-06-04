#pragma once

#include <cstddef>
#include <iosfwd>
#include <string>
#include <vector>

namespace p5 {

struct TabularOptions {
    /// Spaces between columns (default 2).
    size_t columnSpacing = 2;
    /// Terminal width in columns; 0 auto-detects from stdout (fallback 80).
    size_t terminalWidth = 0;
    /// Minimum width per column when shrinking to fit the terminal.
    size_t minColumnWidth = 4;
};

/// Print a table with optional @p headers and @p rows (each row must have the same column count as headers).
void PrintTable(std::ostream &out, const std::vector<std::string> &headers, const std::vector<std::vector<std::string>> &rows, const TabularOptions &options = {});

} // namespace p5
