#include "utils/tabular_renderer.h"

#include "utils/text_width.h"

#include <algorithm>
#include <ostream>
#include <string>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/ioctl.h>
#include <unistd.h>
#endif

namespace p5 {

namespace {

size_t DetectTerminalWidth() {
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO csbi{};
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
        return static_cast<size_t>(csbi.srWindow.Right - csbi.srWindow.Left + 1);
    }
    return 80;
#else
    struct winsize w{};
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0 && w.ws_col > 0) {
        return w.ws_col;
    }
    return 80;
#endif
}

size_t TableWidth(const std::vector<size_t> &colWidths, size_t columnSpacing) {
    if (colWidths.empty()) {
        return 0;
    }
    size_t total = 0;
    for (size_t w : colWidths) {
        total += w;
    }
    total += (colWidths.size() - 1) * columnSpacing;
    return total;
}

void ShrinkColumnsToFit(std::vector<size_t> &colWidths, size_t terminalWidth, size_t columnSpacing, size_t minColumnWidth) {
    while (TableWidth(colWidths, columnSpacing) > terminalWidth) {
        bool reduced = false;
        for (size_t i = 0; i < colWidths.size(); ++i) {
            if (colWidths[i] > minColumnWidth) {
                colWidths[i]--;
                reduced = true;
            }
        }
        if (!reduced) {
            break;
        }
    }
}

void PrintRow(std::ostream &out, const std::vector<std::string> &cells, const std::vector<size_t> &colWidths, size_t columnSpacing) {
    for (size_t i = 0; i < colWidths.size(); ++i) {
        std::string cell = (i < cells.size()) ? cells[i] : "";
        cell = TruncateToDisplayWidth(cell, colWidths[i]);
        out << cell;
        const size_t pad = colWidths[i] > DisplayWidth(cell) ? colWidths[i] - DisplayWidth(cell) : 0;
        if (pad > 0) {
            out << std::string(pad, ' ');
        }
        if (i + 1 < colWidths.size()) {
            out << std::string(columnSpacing, ' ');
        }
    }
    out << '\n';
}

} // namespace

void PrintTable(std::ostream &out, const std::vector<std::string> &headers, const std::vector<std::vector<std::string>> &rows, const TabularOptions &options) {
    const size_t numCols = headers.empty() ? (rows.empty() ? 0 : rows[0].size()) : headers.size();
    if (numCols == 0) {
        return;
    }

    std::vector<size_t> colWidths(numCols, 0);
    const auto updateWidth = [&](size_t col, const std::string &text) {
        if (col < numCols) {
            colWidths[col] = std::max(colWidths[col], DisplayWidth(text));
        }
    };

    for (size_t i = 0; i < headers.size() && i < numCols; ++i) {
        updateWidth(i, headers[i]);
    }
    for (const auto &row : rows) {
        for (size_t i = 0; i < numCols && i < row.size(); ++i) {
            updateWidth(i, row[i]);
        }
    }

    const size_t termWidth = options.terminalWidth > 0 ? options.terminalWidth : DetectTerminalWidth();
    ShrinkColumnsToFit(colWidths, termWidth, options.columnSpacing, options.minColumnWidth);

    if (!headers.empty()) {
        PrintRow(out, headers, colWidths, options.columnSpacing);
    }
    for (const auto &row : rows) {
        PrintRow(out, row, colWidths, options.columnSpacing);
    }
}

} // namespace p5
