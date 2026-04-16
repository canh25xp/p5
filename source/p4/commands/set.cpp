#include "CLI/CLI.hpp"

#include "p5.h"

#include "p4/command_registrars.h"
#include "p4/enviro.h"
#include "p4/error.h"
#include "p4/hostenv.h"
#include "p4/run_forwarded.h"

namespace p4 {

namespace {

/// Same sources as `p4 set` with no arguments: local `Enviro` only (no `ClientApi::Init`, no server).
void print_local_enviro() {
    Enviro env;
    Error e;
    StrBuf cwd;
    HostEnv host;
    host.GetCwd(cwd, &e, &env);
    env.Config(cwd);
    env.LoadEnviro(1);

    if (!P5::P4PORT.empty()) {
        env.Update("P4PORT", P5::P4PORT.c_str());
    }
    if (!P5::P4USER.empty()) {
        env.Update("P4USER", P5::P4USER.c_str());
    }
    if (!P5::P4CLIENT.empty()) {
        env.Update("P4CLIENT", P5::P4CLIENT.c_str());
    }

    env.List(0);
}

} // namespace

static void run_set(CLI::App *cmd) {
    const std::vector<std::string> args = cmd->remaining();

    if (args.empty()) {
        if (!P5::InitializeLibraries()) {
            throw CLI::RuntimeError(1);
        }

        print_local_enviro();

        if (!P5::ShutdownLibraries()) {
            throw CLI::RuntimeError(1);
        }
    } else {
        run_p4_passthrough("set", args);
    }
}

void register_set(CLI::App &app) {
    auto *sub = app.add_subcommand("set", "Show or set Helix client variables (see p4 set)");
    sub->prefix_command();
    sub->callback([sub]() { run_set(sub); });
}

} // namespace p4
