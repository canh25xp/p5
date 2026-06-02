#pragma once

#include <string>

/// Classifies p4 subcommands for client auto-resolution.
namespace ClientCommandPolicy {

/// Returns true when the command never accepts file paths resolved through a
/// client view (file[revRange], file..., etc.) and AutoResolve should be skipped.
/// Commands that optionally take file arguments (e.g. changes, files, labels)
/// are not listed and still auto-resolve when P4CLIENT is unset.
bool SkipsAutoResolve(const std::string &command);

} // namespace ClientCommandPolicy
