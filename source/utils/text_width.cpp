#include "utils/text_width.h"

#ifndef _WIN32
#include <cwchar>
#endif

namespace p5 {

namespace {

#ifdef _WIN32
// WARN: AI generated. Need review
// Fallback wcwidth for Windows: covers ASCII, CJK double-width, combining, and
// common wide ranges.  Matches the POSIX wcwidth contract (0 for non-spacing,
// 1 for single-width, 2 for double-width, -1 for non-printable control chars).
int WcWidthFallback(wint_t c) {
    if (c < 0x20) {
        return -1; // C0 controls
    }
    if (c < 0x7F) {
        return 1; // ASCII printable
    }
    if (c < 0xA0) {
        return -1; // C1 controls / DEL
    }
    // Combining marks (zero-width)
    if (c >= 0x0300 && c <= 0x036F) {
        return 0;
    }
    if (c >= 0x0483 && c <= 0x0489) {
        return 0;
    }
    if (c >= 0x0591 && c <= 0x05BD) {
        return 0;
    }
    if (c == 0x05BF) {
        return 0;
    }
    if (c >= 0x05C1 && c <= 0x05C2) {
        return 0;
    }
    if (c >= 0x05C4 && c <= 0x05C5) {
        return 0;
    }
    if (c == 0x05C7) {
        return 0;
    }
    if (c >= 0x0610 && c <= 0x061A) {
        return 0;
    }
    if (c >= 0x064B && c <= 0x065F) {
        return 0;
    }
    if (c == 0x0670) {
        return 0;
    }
    if (c >= 0x06D6 && c <= 0x06DC) {
        return 0;
    }
    if (c >= 0x06DF && c <= 0x06E4) {
        return 0;
    }
    if (c >= 0x06E7 && c <= 0x06E8) {
        return 0;
    }
    if (c >= 0x06EA && c <= 0x06ED) {
        return 0;
    }
    if (c >= 0x0711) {
        // Fall through to width detection below
    }
    if (c >= 0x1AB0 && c <= 0x1AFF) {
        return 0;
    }
    if (c >= 0x1DC0 && c <= 0x1DFF) {
        return 0;
    }
    if (c >= 0x20D0 && c <= 0x20FF) {
        return 0;
    }
    if (c >= 0xFE00 && c <= 0xFE0F) {
        return 0; // Variation selectors
    }
    if (c >= 0xFE20 && c <= 0xFE2F) {
        return 0;
    }
    if (c >= 0x200B && c <= 0x200F) {
        return 0; // ZW space, ZWNJ, ZWJ, LRM, RLM
    }
    if (c >= 0x2028 && c <= 0x202E) {
        return 0; // bidi controls
    }
    if (c >= 0x2060 && c <= 0x2063) {
        return 0;
    }
    if (c == 0x2066 || c == 0x2067 || c == 0x2068 || c == 0x2069) {
        return 0;
    }
    if (c >= 0xE0100 && c <= 0xE01EF) {
        return 0; // Variation selectors supplement
    }
    if (c >= 0xE0020 && c <= 0xE007F) {
        return 0; // Tags
    }
    // CJK double-width ranges
    if (c >= 0x1100 && c <= 0x115F) {
        return 2; // Hangul Jamo
    }
    if (c >= 0x231A && c <= 0x231B) {
        return 2;
    }
    if (c >= 0x2329 && c <= 0x232A) {
        return 2;
    }
    if (c >= 0x23E9 && c <= 0x23EC) {
        return 2;
    }
    if (c == 0x23F0) {
        return 2;
    }
    if (c == 0x23F3) {
        return 2;
    }
    if (c >= 0x25FD && c <= 0x25FE) {
        return 2;
    }
    if (c >= 0x2614 && c <= 0x2615) {
        return 2;
    }
    if (c >= 0x2648 && c <= 0x2653) {
        return 2;
    }
    if (c == 0x267F) {
        return 2;
    }
    if (c >= 0x2693 && c <= 0x2693) {
        return 2;
    }
    if (c >= 0x26A1 && c <= 0x26A1) {
        return 2;
    }
    if (c >= 0x26AA && c <= 0x26AB) {
        return 2;
    }
    if (c >= 0x26BD && c <= 0x26BE) {
        return 2;
    }
    if (c >= 0x26C4 && c <= 0x26C5) {
        return 2;
    }
    if (c == 0x26CE) {
        return 2;
    }
    if (c == 0x26D4) {
        return 2;
    }
    if (c == 0x26EA) {
        return 2;
    }
    if (c >= 0x26F2 && c <= 0x26F3) {
        return 2;
    }
    if (c >= 0x26F5 && c <= 0x26F5) {
        return 2;
    }
    if (c >= 0x26FA && c <= 0x26FA) {
        return 2;
    }
    if (c >= 0x26FD && c <= 0x26FD) {
        return 2;
    }
    if (c >= 0x2702 && c <= 0x2702) {
        return 2;
    }
    if (c >= 0x2705 && c <= 0x2705) {
        return 2;
    }
    if (c >= 0x2708 && c <= 0x270D) {
        return 2;
    }
    if (c >= 0x270F && c <= 0x270F) {
        return 2;
    }
    if (c == 0x2712) {
        return 2;
    }
    if (c == 0x2714) {
        return 2;
    }
    if (c == 0x2716) {
        return 2;
    }
    if (c == 0x271D) {
        return 2;
    }
    if (c == 0x2721) {
        return 2;
    }
    if (c >= 0x2728 && c <= 0x2728) {
        return 2;
    }
    if (c >= 0x2733 && c <= 0x2734) {
        return 2;
    }
    if (c >= 0x2744 && c <= 0x2744) {
        return 2;
    }
    if (c >= 0x2747 && c <= 0x2747) {
        return 2;
    }
    if (c >= 0x274C && c <= 0x274C) {
        return 2;
    }
    if (c >= 0x274E && c <= 0x274E) {
        return 2;
    }
    if (c >= 0x2753 && c <= 0x2755) {
        return 2;
    }
    if (c == 0x2757) {
        return 2;
    }
    if (c >= 0x2763 && c <= 0x2764) {
        return 2;
    }
    if (c >= 0x2795 && c <= 0x2797) {
        return 2;
    }
    if (c == 0x27A1) {
        return 2;
    }
    if (c >= 0x27B0 && c <= 0x27B0) {
        return 2;
    }
    if (c >= 0x27BF && c <= 0x27BF) {
        return 2;
    }
    if (c >= 0x2934 && c <= 0x2935) {
        return 2;
    }
    if (c >= 0x2B05 && c <= 0x2B07) {
        return 2;
    }
    if (c >= 0x2B1B && c <= 0x2B1C) {
        return 2;
    }
    if (c >= 0x2B50 && c <= 0x2B50) {
        return 2;
    }
    if (c >= 0x2B55 && c <= 0x2B55) {
        return 2;
    }
    if (c >= 0x2B1B && c <= 0x2B1C) {
        return 2;
    }
    if (c >= 0x2E80 && c <= 0x303E) {
        return 2; // CJK misc
    }
    if (c >= 0x3041 && c <= 0x3096) {
        return 2; // Hiragana
    }
    if (c >= 0x3099 && c <= 0x30FF) {
        return 2; // Katakana
    }
    if (c >= 0x3105 && c <= 0x312F) {
        return 2; // Bopomofo
    }
    if (c >= 0x3131 && c <= 0x318E) {
        return 2; // Hangul Compatibility Jamo
    }
    if (c >= 0x3190 && c <= 0x3247) {
        return 2;
    }
    if (c >= 0x3250 && c <= 0x4DBF) {
        return 2; // CJK Unified Ideographs Extension A
    }
    if (c >= 0x4E00 && c <= 0x9FFF) {
        return 2; // CJK Unified Ideographs
    }
    if (c >= 0xA960 && c <= 0xA97C) {
        return 2;
    }
    if (c >= 0xAC00 && c <= 0xD7A3) {
        return 2; // Hangul Syllables
    }
    if (c >= 0xF900 && c <= 0xFAFF) {
        return 2; // CJK Compatibility Ideographs
    }
    if (c >= 0xFE10 && c <= 0xFE19) {
        return 2;
    }
    if (c >= 0xFE30 && c <= 0xFE6B) {
        return 2;
    }
    if (c >= 0xFF01 && c <= 0xFF60) {
        return 2; // Fullwidth forms
    }
    if (c >= 0xFFE0 && c <= 0xFFE6) {
        return 2;
    }
    if (c >= 0x1B000 && c <= 0x1B0FF) {
        return 2; // Kana Supplement
    }
    if (c >= 0x1F100 && c <= 0x1F1AD) {
        return 2;
    }
    if (c >= 0x1F1E6 && c <= 0x1F1FF) {
        return 2; // Regional indicators
    }
    if (c >= 0x1F200 && c <= 0x1F251) {
        return 2; // Emoji
    }
    if (c >= 0x1F300 && c <= 0x1F64F) {
        return 2; // Emoji
    }
    if (c >= 0x1F680 && c <= 0x1F6FF) {
        return 2; // Emoji
    }
    if (c >= 0x1F900 && c <= 0x1F9FF) {
        return 2; // Emoji
    }
    if (c >= 0x1FA00 && c <= 0x1FA6F) {
        return 2;
    }
    if (c >= 0x1FA70 && c <= 0x1FAFF) {
        return 2;
    }
    if (c >= 0x20000 && c <= 0x2FFFD) {
        return 2; // CJK Extension B-I
    }
    if (c >= 0x30000 && c <= 0x3FFFD) {
        return 2; // CJK Extension G
    }
    // Default: single-width for other printable characters
    return 1;
}
#endif

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
#ifdef _WIN32
    const int width = WcWidthFallback(static_cast<wint_t>(codepoint));
#else
    const int width = wcwidth(static_cast<wint_t>(codepoint));
#endif
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

std::string TruncateToDisplayWidth(const std::string &text, size_t maxColumns) {
    if (maxColumns == 0) {
        return {};
    }
    if (DisplayWidth(text) <= maxColumns) {
        return text;
    }

    static constexpr char kEllipsis[] = "...";
    const size_t ellipsisWidth = 3;
    if (maxColumns <= ellipsisWidth) {
        std::string out;
        size_t used = 0;
        for (size_t i = 0; i < text.size() && used < maxColumns;) {
            char32_t codepoint = 0;
            size_t bytes = 0;
            if (!DecodeUtf8(text, i, codepoint, bytes)) {
                out.push_back(text[i]);
                used += 1;
                i += 1;
                continue;
            }
            const int w = CodepointWidth(codepoint);
            if (used + static_cast<size_t>(w) > maxColumns) {
                break;
            }
            out.append(text, i, bytes);
            used += static_cast<size_t>(w);
            i += bytes;
        }
        return out;
    }

    const size_t budget = maxColumns - ellipsisWidth;
    std::string out;
    size_t used = 0;
    for (size_t i = 0; i < text.size();) {
        char32_t codepoint = 0;
        size_t bytes = 0;
        if (!DecodeUtf8(text, i, codepoint, bytes)) {
            if (used + 1 > budget) {
                break;
            }
            out.push_back(text[i]);
            used += 1;
            i += 1;
            continue;
        }
        const int w = CodepointWidth(codepoint);
        if (used + static_cast<size_t>(w) > budget) {
            break;
        }
        out.append(text, i, bytes);
        used += static_cast<size_t>(w);
        i += bytes;
    }
    out += kEllipsis;
    return out;
}

} // namespace p5
