#pragma once

#include <string>
#include <vector>

#include "types/spec.h"

namespace p5 {

// Represents a Perforce client specification with all typed fields.
// Mirrors the structure returned by `p4 client -o` command.
struct ClientSpec {
    std::string client;
    std::string update;
    std::string access;
    std::string owner;
    std::string host;
    std::string description;
    std::string root;
    std::string options;
    std::string submitOptions;
    std::string lineEnd;
    std::vector<std::string> view;
    std::string type;
    std::string backup;

    // Print formatted client spec to stdout
    void Print() const;
};

// Parse a Spec into a ClientSpec struct
ClientSpec ParseClientSpec(const Spec &spec);

} // namespace p5
