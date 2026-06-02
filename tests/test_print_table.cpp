#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/ioctl.h>
#include <unistd.h>
#endif

// Get terminal width
int GetTerminalWidth() {
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
        return csbi.srWindow.Right - csbi.srWindow.Left + 1;
    }
    return 80;
#else
    struct winsize w{};
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0) {
        return w.ws_col;
    }
    return 80;
#endif
}

// Truncate with ellipsis
std::string Truncate(const std::string &s, size_t max_width) {
    if (s.size() <= max_width)
        return s;
    if (max_width <= 3)
        return s.substr(0, max_width);
    return s.substr(0, max_width - 3) + "...";
}

// Print table
void PrintTable(const std::vector<std::vector<std::string>> &rows) {
    if (rows.empty())
        return;

    const int term_width = GetTerminalWidth();
    const size_t cols = rows[0].size();

    // Step 1: compute max width per column
    std::vector<size_t> col_widths(cols, 0);
    for (const auto &row : rows) {
        for (size_t i = 0; i < cols; ++i) {
            if (i < row.size())
                col_widths[i] = std::max(col_widths[i], row[i].size());
        }
    }

    // Step 2: total width (including spacing)
    size_t total_width = 0;
    const size_t padding = 2; // spaces between columns
    for (auto w : col_widths)
        total_width += w;
    total_width += (cols - 1) * padding;

    // Step 3: shrink if needed
    if (total_width > (size_t)term_width) {
        size_t overflow = total_width - term_width;

        // shrink columns proportionally
        while (overflow > 0) {
            for (size_t i = 0; i < cols && overflow > 0; ++i) {
                if (col_widths[i] > 4) { // don't shrink too much
                    col_widths[i]--;
                    overflow--;
                }
            }
        }
    }

    // Step 4: print rows
    for (const auto &row : rows) {
        for (size_t i = 0; i < cols; ++i) {
            std::string cell = (i < row.size()) ? row[i] : "";
            cell = Truncate(cell, col_widths[i]);

            std::cout << cell;

            // padding
            if (cell.size() < col_widths[i]) {
                std::cout << std::string(col_widths[i] - cell.size(), ' ');
            }

            if (i != cols - 1)
                std::cout << std::string(padding, ' ');
        }
        std::cout << "\n";
    }
}

int main() {
    std::vector<std::vector<std::string>> rows = {
        {"Client", "Root", "Owner"},
        {"admin_NewJeans", "/home/vancanh-ng/NewJeans", "p4admin"},
        {"bunnies-desktop-debian_NewJeans", "/home/michael/Documents/NewJeans", "bunnies"},
        {"bunnies-desktop-debian_p5", "/home/michael/projects/p5", "bunnies"},
        {"vancanh-ng_NewJeans", "/home/vancanh-ng/NewJeans", "bunnies"},
        {"vancanh-ng_p5", "/home/vancanh-ng/projects/p5", "bunnies"}};

    PrintTable(rows);
}
