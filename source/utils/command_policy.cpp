#include "utils/command_policy.h"

#include <string>
#include <unordered_map>
#include <unordered_set>

namespace CommandPolicy {

namespace {

const std::unordered_set<std::string> &AllCommandsImpl() {
    static const std::unordered_set<std::string> commands = {
        // File operations
        "add",
        "delete",
        "edit",
        "move",
        "reopen",
        "revert",
        "lock",
        "unlock",
        // Changelist operations
        "change",
        "changes",
        "describe",
        "shelve",
        "unshelve",
        "reshelve",
        "submit",
        "resolve",
        "resolved",
        // Sync / workspace state
        "sync",
        "update",
        "flush",
        "clean",
        "reconcile",
        "fast-reconcile",
        "status",
        "have",
        "opened",
        "where",
        "ignores",
        // Client / workspace
        "client",
        "map",
        "clients",
        // Depot / file info
        "files",
        "fstat",
        "filelog",
        "annotate",
        "diff",
        "diff2",
        "print",
        "dirs",
        "sizes",
        "grep",
        "cstat",
        // Integration
        "integrate",
        "integrated",
        "interchanges",
        "merge",
        "copy",
        "undo",
        // Branch / label / tag
        "branch",
        "branches",
        "label",
        "labels",
        "labelsync",
        "list",
        "tag",
        // Stream
        "stream",
        "streams",
        "streamlog",
        "streamspec",
        "switch",
        "istat",
        "populate",
        "prune",
        // Job
        "job",
        "jobs",
        "fix",
        "fixes",
        // User / group
        "users",
        "user",
        "group",
        "groups",
        "passwd",
        // Depot / repo admin
        "depot",
        "depots",
        "repo",
        "repos",
        // Counter / key
        "counter",
        "counters",
        "key",
        "keys",
        // Protection / review
        "protect",
        "protects",
        "review",
        "reviews",
        "attribute",
        // Auth
        "login",
        "login2",
        "logout",
        "tickets",
        "trust",
        // Info / local
        "info",
        "logger",
        "aliases",
        "help",
        "set",
        "map",
    };
    return commands;
}

const std::unordered_map<std::string, std::string> &AliasMap() {
    static const std::unordered_map<std::string, std::string> aliases = {
        {"rename", "move"},
        {"changelist", "change"},
        {"changelists", "changes"},
        {"rec", "reconcile"},
        {"fast-rec", "fast-reconcile"},
        {"workspace", "client"},
        {"workspaces", "clients"},
    };
    return aliases;
}

const std::unordered_set<std::string> &NoOnline() {
    static const std::unordered_set<std::string> commands = {
        "help",
        "aliases",
        "set",
    };
    return commands;
}

// Commands that never accept file[revRange] / client-view file arguments.
// If a command can take file paths resolved through P4CLIENT (e.g. p4 changes
// //client/path), it must not be listed here so auto-resolve still runs.
const std::unordered_set<std::string> &NoNeedClientResolve() {
    static const std::unordered_set<std::string> commands = {
        // Auth / session
        "login",
        "login2",
        "logout",
        "tickets",
        "trust",
        "passwd",
        // Users / groups
        "users",
        "user",
        "groups",
        "group",
        // Changelist query
        "describe",
        // Spec / server metadata (no client-view file args)
        "branches",
        "branch",
        "label",
        "depots",
        "depot",
        "repos",
        "repo",
        "clients",
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
        // Offline (also skip resolve)
        "help",
        "aliases",
        "set",
    };
    return commands;
}

const std::unordered_set<std::string> &ReadOnlySeed() {
    static const std::unordered_set<std::string> commands = {
        "changes",
        "clients",
        "describe",
        "files",
        "fstat",
        "filelog",
        "annotate",
        "print",
        "dirs",
        "sizes",
        "grep",
        "have",
        "opened",
        "where",
        "ignores",
        "cstat",
        "istat",
        "branches",
        "depots",
        "repos",
        "streams",
        "labels",
        "jobs",
        "fixes",
        "users",
        "groups",
        "counters",
        "keys",
        "protects",
        "reviews",
        "integrated",
        "interchanges",
        "resolved",
        "info",
        "logger",
        "tickets",
    };
    return commands;
}

const std::unordered_set<std::string> &NoReadOnly() {
    static const std::unordered_set<std::string> computed = [] {
        std::unordered_set<std::string> deny = AllCommandsImpl();
        for (const auto &cmd : ReadOnlySeed()) {
            deny.erase(cmd);
        }
        return deny;
    }();
    return computed;
}

} // namespace

const std::unordered_set<std::string> &AllCommands() {
    return AllCommandsImpl();
}

std::string NormalizeCommand(const std::string &command) {
    const auto it = AliasMap().find(command);
    if (it != AliasMap().end()) {
        return it->second;
    }
    return command;
}

bool IsOnline(const std::string &command) {
    return NoOnline().count(NormalizeCommand(command)) == 0;
}

bool IsOffline(const std::string &command) {
    return !IsOnline(command);
}

bool NeedsClientResolve(const std::string &command) {
    const std::string cmd = NormalizeCommand(command);
    if (cmd.empty()) {
        return false;
    }
    return NoNeedClientResolve().count(cmd) == 0;
}

bool IsReadOnly(const std::string &command) {
    const std::string cmd = NormalizeCommand(command);
    if (AllCommandsImpl().count(cmd) == 0) {
        return false;
    }
    return NoReadOnly().count(cmd) == 0;
}

} // namespace CommandPolicy
