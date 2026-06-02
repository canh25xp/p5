#include "utils/client_command_policy.h"

#include <string>
#include <unordered_set>

namespace ClientCommandPolicy {

namespace {

const std::unordered_set<std::string> &NoClientCommands() {
    static const std::unordered_set<std::string> commands = {
        // Help / local
        "help",
        "aliases",
        "set",
        // Auth / session
        "login",
        "login2",
        "logout",
        "tickets",
        "trust",
        // Users / groups
        "users",
        "user",
        "groups",
        "group",
        "passwd",
        // Changelist read-only
        "changes",
        "describe",
        // Spec / server metadata
        "branches",
        "branch",
        "labels",
        "label",
        "list",
        "tag",
        "depots",
        "depot",
        "repos",
        "repo",
        "clients",
        "client",
        "streams",
        "stream",
        "streamlog",
        "streamspec",
        "jobs",
        "job",
        "fixes",
        "fix",
        "counters",
        "counter",
        "keys",
        "key",
        "protect",
        "protects",
        "review",
        "reviews",
        "attribute",
        // Info / logging
        "info",
        "logger",
        // Depot-side file queries
        "files",
        "fstat",
        "filelog",
        "print",
        "dirs",
        "sizes",
        "grep",
        "annotate",
        "diff2",
        "integrated",
        "interchanges",
    };
    return commands;
}

} // namespace

bool SkipsAutoResolve(const std::string &command) {
    if (command.empty()) {
        return true;
    }
    return NoClientCommands().count(command) != 0;
}

} // namespace ClientCommandPolicy
