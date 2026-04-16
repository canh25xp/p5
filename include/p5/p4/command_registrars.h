#pragma once

#include "CLI/CLI.hpp"

namespace p4 {

void register_info(CLI::App &app);
void register_sync(CLI::App &app);
void register_changes(CLI::App &app);
void register_describe(CLI::App &app);
void register_files(CLI::App &app);
void register_clients(CLI::App &app);
void register_users(CLI::App &app);

} // namespace p4
