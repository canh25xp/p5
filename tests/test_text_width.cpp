// Standalone test for DisplayWidth.
// Compile manually:
//   g++ -std=c++17 -Iinclude/p5 -o test_text_width tests/test_text_width.cpp source/utils/text_width.cpp && ./test_text_width

#include "utils/text_width.h"

#include <cassert>
#include <iostream>
#include <string>

#define ASSERT_EQ(a, b)                                                  \
    do {                                                                 \
        if ((a) != (b)) {                                                \
            std::cerr << "FAIL " << __FILE__ << ":" << __LINE__ << " — " \
                      << #a << " != " << #b << "\n";                     \
            return 1;                                                    \
        }                                                                \
    } while (0)

int main() {
    ASSERT_EQ(p5::DisplayWidth("abc"), 3u);
    ASSERT_EQ(p5::DisplayWidth(""), 0u);

    // NBSP is one column but two UTF-8 bytes (alignment bug from NOTE.md).
    const std::string nbsp_name = "Samsung\u00a0Electronics";
    const std::string space_name = "Samsung Electronics";
    ASSERT_EQ(p5::DisplayWidth(nbsp_name), p5::DisplayWidth(space_name));

    std::cout << "All DisplayWidth tests passed.\n";
    return 0;
}
