#include "utils/client_command_policy.h"

#include <string>
#include <unordered_set>

namespace ClientCommandPolicy {

namespace {

// Commands that never accept file[revRange] / client-view file arguments.
// If a command can take file paths resolved through P4CLIENT (e.g. p4 changes
// //client/path), it must not be listed here so AutoResolve still runs.
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
        "describe",
        // Spec / server metadata
        "branches",
        "branch",
        "label",
        "depots",
        "depot",
        "repos",
        "repo",
        "clients",
        "client",
        "stream",
        "streamlog",
        "streamspec",
        "job",
        "fix",
        "counters",
        "counter",
        "keys",
        "key",
        "protect",
        "review",
        // Info / logging
        "info",
        "logger",
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
