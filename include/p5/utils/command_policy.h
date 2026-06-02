#pragma once

#include <string>
#include <unordered_set>

/// Classifies p5/p4 subcommands using a master command list and small deny sets.
namespace CommandPolicy {

/// Canonical subcommand names registered by p5 (primary names only; aliases via NormalizeCommand).
const std::unordered_set<std::string> &AllCommands();

/// Map CLI alias to canonical subcommand name; returns input unchanged when not an alias.
std::string NormalizeCommand(const std::string &command);

/// True when the command uses the server (default); false for offline/client-side only.
bool IsOnline(const std::string &command);

/// True for offline/client-side commands (help, aliases, set).
bool IsOffline(const std::string &command);

/// True when auto-resolve should run because P4CLIENT is unset (default for non-empty commands).
bool NeedsClientResolve(const std::string &command);

/// True for unambiguous query/list commands (conservative; excludes dual-purpose spec editors).
bool IsReadOnly(const std::string &command);

} // namespace CommandPolicy
