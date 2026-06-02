#pragma once

#include <cstddef>
#include <string>

namespace p5 {

/// Terminal display width (wcwidth semantics), not UTF-8 byte length.
size_t DisplayWidth(const std::string &text);

} // namespace p5
