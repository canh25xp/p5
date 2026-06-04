// Standalone test for tabular renderer.
// Compile manually:
//   g++ -std=c++17 -Iinclude/p5 -o test_tabular_renderer tests/test_tabular_renderer.cpp \
//       source/utils/tabular_renderer.cpp source/utils/text_width.cpp && ./test_tabular_renderer

#include "utils/tabular_renderer.h"
#include "utils/text_width.h"

#include <cassert>
#include <iostream>
#include <sstream>
#include <string>

#define ASSERT_EQ(a, b)                                                  \
    do {                                                                 \
        if ((a) != (b)) {                                                \
            std::cerr << "FAIL " << __FILE__ << ":" << __LINE__ << " — " \
                      << #a << " != " << #b << "\n";                     \
            return 1;                                                    \
        }                                                                \
    } while (0)

#define ASSERT_CONTAINS(haystack, needle)                              \
    do {                                                                 \
        if ((haystack).find(needle) == std::string::npos) {              \
            std::cerr << "FAIL " << __FILE__ << ":" << __LINE__ << " — " \
                      << #haystack << " does not contain " << #needle << "\n"; \
            return 1;                                                    \
        }                                                                \
    } while (0)

int main() {
    // TruncateToDisplayWidth basics
    ASSERT_EQ(p5::TruncateToDisplayWidth("hello", 10), "hello");
    ASSERT_EQ(p5::TruncateToDisplayWidth("hello world", 8), "hello...");

    const std::string nbsp_name = "Samsung\u00a0Electronics";
    ASSERT_EQ(p5::DisplayWidth(nbsp_name), p5::DisplayWidth("Samsung Electronics"));

    // Header participates in column width
    {
        std::ostringstream out;
        p5::TabularOptions opts;
        opts.terminalWidth = 200;
        const std::vector<std::string> headers = {"User", "FullName", "Email"};
        const std::vector<std::vector<std::string>> rows = {{"ab", "cd", "ef"}};
        p5::PrintTable(out, headers, rows, opts);
        const std::string rendered = out.str();
        ASSERT_CONTAINS(rendered, "FullName");
        ASSERT_CONTAINS(rendered, "ab");
        // FullName is wider than "cd", so padding after "cd" should exist
        const auto pos = rendered.find("cd");
        ASSERT_EQ(rendered.find("ef", pos), pos + 10); // "cd" + 8 spaces + "ef" (FullName width 8)
    }

    // NBSP alignment: two rows with NBSP vs space should align in second column
    {
        std::ostringstream out;
        p5::TabularOptions opts;
        opts.terminalWidth = 200;
        const std::vector<std::string> headers = {"A", "B"};
        const std::vector<std::vector<std::string>> rows = {
            {"x", nbsp_name},
            {"y", "Samsung Electronics"},
        };
        p5::PrintTable(out, headers, rows, opts);
        const std::string rendered = out.str();
        const auto nl1 = rendered.find('\n');
        const auto nl2 = rendered.find('\n', nl1 + 1);
        const std::string row1 = rendered.substr(nl1 + 1, nl2 - nl1 - 1);
        const std::string row2 = rendered.substr(nl2 + 1, rendered.find('\n', nl2 + 1) - nl2 - 1);
        const auto b1 = row1.find(nbsp_name);
        const auto b2 = row2.find("Samsung Electronics");
        ASSERT_EQ(b1, b2);
    }

    // Terminal width forces ellipsis on long cell
    {
        std::ostringstream out;
        p5::TabularOptions opts;
        opts.terminalWidth = 30;
        opts.minColumnWidth = 4;
        const std::vector<std::string> headers = {"Client", "Root"};
        const std::vector<std::vector<std::string>> rows = {
            {"short", "/very/long/path/that/should/be/truncated/with/ellipsis"},
        };
        p5::PrintTable(out, headers, rows, opts);
        ASSERT_CONTAINS(out.str(), "...");
    }

    // Narrow terminal: still produces header and data rows (cells may truncate heavily)
    {
        std::ostringstream out;
        p5::TabularOptions opts;
        opts.terminalWidth = 20;
        opts.minColumnWidth = 3;
        const std::vector<std::string> headers = {"Change", "Date", "User"};
        const std::vector<std::vector<std::string>> rows = {{"12345", "2024/01/01", "alice"}};
        p5::PrintTable(out, headers, rows, opts);
        const std::string rendered = out.str();
        ASSERT_EQ(rendered.empty(), false);
        ASSERT_EQ(rendered.find('\n') != std::string::npos, true);
        ASSERT_EQ(rendered.find('\n', rendered.find('\n') + 1) != std::string::npos, true);
    }

    std::cout << "All tabular renderer tests passed.\n";
    return 0;
}
