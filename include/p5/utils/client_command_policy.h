#pragma once

#include <string>

/// Classifies p4 subcommands for client auto-resolution.
namespace ClientCommandPolicy {

/// Returns true when the command does not require a workspace client and
/// AutoResolve should be skipped.
bool SkipsAutoResolve(const std::string &command);

} // namespace ClientCommandPolicy
