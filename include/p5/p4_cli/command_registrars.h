#pragma once

#include "CLI/CLI.hpp"

namespace p4_cli {

void register_info(CLI::App &app);
void register_sync(CLI::App &app);
void register_changes(CLI::App &app);
void register_describe(CLI::App &app);
void register_files(CLI::App &app);

} // namespace p4_cli
