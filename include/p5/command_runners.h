#pragma once

#include <string>
#include <vector>

#include "CLI/CLI.hpp"

void run_users(const std::vector<std::string> &);

void register_set(CLI::App &app);
