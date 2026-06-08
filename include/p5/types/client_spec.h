#pragma once

#include <string>
#include <vector>
#include <iosfwd>

#include "types/spec.h"

namespace p5 {

// Represents a Perforce client specification with all typed fields.
// Mirrors the structure returned by `p4 client -o` command.
class ClientSpec {
public:
    ClientSpec() = default;
    explicit ClientSpec(const Spec &spec);

    // Public data members for direct access
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

    // Friend declaration for stream output operator
    friend std::ostream &operator<<(std::ostream &os, const ClientSpec &clientSpec);
};

} // namespace p5
