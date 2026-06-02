#include "utils/text_width.h"

#include <cwchar>

namespace p5 {

namespace {

bool DecodeUtf8(const std::string &text, size_t index, char32_t &codepoint, size_t &bytes) {
    if (index >= text.size()) {
        return false;
    }
    const unsigned char b0 = static_cast<unsigned char>(text[index]);
    if (b0 < 0x80) {
        codepoint = b0;
        bytes = 1;
        return true;
    }
    if ((b0 & 0xE0) == 0xC0 && index + 1 < text.size()) {
        const unsigned char b1 = static_cast<unsigned char>(text[index + 1]);
        if ((b1 & 0xC0) == 0x80) {
            codepoint = ((b0 & 0x1F) << 6) | (b1 & 0x3F);
            bytes = 2;
            return true;
        }
    }
    if ((b0 & 0xF0) == 0xE0 && index + 2 < text.size()) {
        const unsigned char b1 = static_cast<unsigned char>(text[index + 1]);
        const unsigned char b2 = static_cast<unsigned char>(text[index + 2]);
        if ((b1 & 0xC0) == 0x80 && (b2 & 0xC0) == 0x80) {
            codepoint = ((b0 & 0x0F) << 12) | ((b1 & 0x3F) << 6) | (b2 & 0x3F);
            bytes = 3;
            return true;
        }
    }
    if ((b0 & 0xF8) == 0xF0 && index + 3 < text.size()) {
        const unsigned char b1 = static_cast<unsigned char>(text[index + 1]);
        const unsigned char b2 = static_cast<unsigned char>(text[index + 2]);
        const unsigned char b3 = static_cast<unsigned char>(text[index + 3]);
        if ((b1 & 0xC0) == 0x80 && (b2 & 0xC0) == 0x80 && (b3 & 0xC0) == 0x80) {
            codepoint = ((b0 & 0x07) << 18) | ((b1 & 0x3F) << 12) | ((b2 & 0x3F) << 6) | (b3 & 0x3F);
            bytes = 4;
            return true;
        }
    }
    return false;
}

int CodepointWidth(char32_t codepoint) {
    if (codepoint < 0 || codepoint > 0x10FFFF) {
        return 1;
    }
    const int width = wcwidth(static_cast<wint_t>(codepoint));
    if (width > 0) {
        return width;
    }
    if (width == 0) {
        return 0;
    }
    // wcwidth -1: libc marks non-printable, but many still occupy one cell (e.g. NBSP U+00A0).
    if (codepoint >= 0xA0) {
        return 1;
    }
    return 0;
}

} // namespace

size_t DisplayWidth(const std::string &text) {
    size_t width = 0;
    for (size_t i = 0; i < text.size();) {
        char32_t codepoint = 0;
        size_t bytes = 0;
        if (!DecodeUtf8(text, i, codepoint, bytes)) {
            width += 1;
            i += 1;
            continue;
        }
        width += static_cast<size_t>(CodepointWidth(codepoint));
        i += bytes;
    }
    return width;
}

} // namespace p5
