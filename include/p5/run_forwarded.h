#pragma once

#include <string>
#include <vector>

class P5;

/// Copy parsed global option values into `P5` statics before subcommand callbacks run.
void apply_global_options(const std::string &user, const std::string &port, const std::string &client, const std::vector<std::string> &protocol_z);

void run_forwarded(P5 &p5, const char *command, const std::vector<std::string> &args);

void run_p4_passthrough(const char *command, const std::vector<std::string> &args);
