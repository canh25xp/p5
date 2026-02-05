#include "cli_helpers.h"
#include <string>
#include <vector>

std::vector<std::string> SplitCommandLine(const std::string &input) {
    std::vector<std::string> args;
    std::string current;
    bool inQuotes = false;

    for (size_t i = 0; i < input.size(); ++i) {
        char c = input[i];

        if (c == '"') {
            inQuotes = !inQuotes;
            continue;
        }

        if (std::isspace(static_cast<unsigned char>(c)) && !inQuotes) {
            if (!current.empty()) {
                args.push_back(current);
                current.clear();
            }
        } else {
            current += c;
        }
    }

    if (!current.empty()) {
        args.push_back(current);
    }

    return args;
}

