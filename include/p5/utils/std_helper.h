#pragma once

#include <array>
#include <cstdint>
#include <string>

std::string ToLower(std::string s);

std::string ShellQuote(const std::string &arg);

bool ParseDigestHex(const char *hex, std::array<uint8_t, 16> &out);
