#pragma once

#include <string>
#include <vector>

class P5;

/// Copy parsed global `-u` / `-p` / `-c` values (and any CLI11 `envname` fill) into `P5` statics.
/// Intended for use from the root app's `parse_complete_callback` so it runs before subcommand callbacks.
void apply_connection_options(const std::string &user, const std::string &port, const std::string &client);

/// Parse `-z` arguments (`key` or `key=value`) into `P5` statics for `ClientApi::SetProtocol`.
void apply_protocol_options(const std::vector<std::string> &protocol_z);

void run_forwarded(P5 &p5, const char *command, const std::vector<std::string> &args);

void run_p4_passthrough(const char *command, const std::vector<std::string> &args);
