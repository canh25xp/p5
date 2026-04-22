#pragma once

namespace CLI {
class App;
}
class Options;

void register_set(CLI::App &app, const Options &options);
