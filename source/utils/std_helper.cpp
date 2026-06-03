#include "utils/std_helper.h"

#include <algorithm>
#include <cctype>
#include <cstring>

std::string ToLower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return std::tolower(c); });
    return s;
}

std::string ShellQuote(const std::string &arg) {
    std::string out = "'";
    for (char c : arg) {
        if (c == '\'') {
            out += "'\\''";
        } else {
            out += c;
        }
    }
    out += "'";
    return out;
}

bool ParseDigestHex(const char *hex, std::array<uint8_t, 16> &out) {
    if (!hex || std::strlen(hex) != 32) {
        return false;
    }
    for (int i = 0; i < 16; ++i) {
        char buf[3] = {hex[i * 2], hex[i * 2 + 1], '\0'};
        char *end = nullptr;
        unsigned long v = std::strtoul(buf, &end, 16);
        if (end != buf + 2) {
            return false;
        }
        out[static_cast<size_t>(i)] = static_cast<uint8_t>(v);
    }
    return true;
}
