// Standalone test for SplitCommandLine.
// Compile manually:
//   g++ -std=c++17 -Iinclude/p5 -o test_cli_helpers tests/test_cli_helpers.cpp source/utils/cli_helpers.cpp && ./test_cli_helpers

#include "utils/cli_helpers.h"

#include <cassert>
#include <iostream>
#include <string>
#include <vector>

#define ASSERT_EQ(a, b)                                                  \
    do {                                                                 \
        if ((a) != (b)) {                                                \
            std::cerr << "FAIL " << __FILE__ << ":" << __LINE__ << " — " \
                      << #a << " != " << #b << "\n";                     \
            return 1;                                                    \
        }                                                                \
    } while (0)

#define ASSERT_VEC_EQ(actual, ...)                      \
    do {                                                \
        std::vector<std::string> expected{__VA_ARGS__}; \
        ASSERT_EQ(actual, expected);                    \
    } while (0)

// g++ -std=c++17 -Iinclude/p5 -o test_cli_helpers tests/test_cli_helpers.cpp source/utils/cli_helpers.cpp && ./test_cli_helpers
int main() {
    // Basic test
    ASSERT_VEC_EQ(SplitCommandLine("p4 changes -m1 depot/path/..."), "p4", "changes", "-m1", "depot/path/...");

    // Empty input returns no args.
    ASSERT_VEC_EQ(SplitCommandLine(""), );

    // Single word.
    ASSERT_VEC_EQ(SplitCommandLine("hello"), "hello");

    // Multiple words separated by spaces.
    ASSERT_VEC_EQ(SplitCommandLine("foo bar baz"), "foo", "bar", "baz");

    // Multiple consecutive spaces are collapsed.
    ASSERT_VEC_EQ(SplitCommandLine("  a   b  "), "a", "b");

    // Leading and trailing spaces are ignored.
    ASSERT_VEC_EQ(SplitCommandLine("  hello  "), "hello");

    // Tabs and other whitespace are treated as delimiters.
    ASSERT_VEC_EQ(SplitCommandLine("one\ttwo\nthree"), "one", "two", "three");

    // Quoted string preserves internal spaces; quotes are stripped.
    ASSERT_VEC_EQ(SplitCommandLine("\"hello world\""), "hello world");

    // Quoted segment in the middle of a command line.
    ASSERT_VEC_EQ(
        SplitCommandLine("cmd \"arg with spaces\" tail"),
        "cmd", "arg with spaces", "tail");

    // Two separate quoted arguments.
    ASSERT_VEC_EQ(
        SplitCommandLine("\"first arg\" \"second arg\""),
        "first arg", "second arg");

    // Mixed quoted and unquoted tokens.
    ASSERT_VEC_EQ(
        SplitCommandLine("cmd --flag \"value with spaces\" plain"),
        "cmd", "--flag", "value with spaces", "plain");

    // Empty quoted string produces no argument (current stays empty, never pushed).
    ASSERT_VEC_EQ(SplitCommandLine("\"\""), );

    // Unclosed quote: everything after the opening quote is one arg.
    ASSERT_VEC_EQ(SplitCommandLine("\"unclosed quote"), "unclosed quote");

    // Quote in the middle of a word glues the parts together.
    ASSERT_VEC_EQ(SplitCommandLine("pre\"mid\"post"), "premidpost");

    // Only whitespace returns no args.
    ASSERT_VEC_EQ(SplitCommandLine("   \t  \n  "), );

    std::cout << "All SplitCommandLine tests passed.\n";
    return 0;
}
