#pragma once

#include <cstddef>
#include <string>

namespace p5 {

/// Terminal display width (wcwidth semantics), not UTF-8 byte length.
size_t DisplayWidth(const std::string &text);

/// Truncate to @p maxColumns display columns, appending "..." when shortened.
std::string TruncateToDisplayWidth(const std::string &text, size_t maxColumns);

} // namespace p5
