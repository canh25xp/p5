#include <string>
#include <vector>

#include "CLI/CLI.hpp"
#include "dbg.h"

#include "log.h"
#include "p4_cli/registry.h"
#include "p4_cli/run_forwarded.h"
#include "p5.h"

int main(int argc, char **argv);
int main(int argc, char **argv) {
    CLI::App app("p5: Complementary p4 command-line tools");

    argv = app.ensure_utf8(argv);

    std::string user = "";
    std::string port = "";
    std::string client = "";

    app.add_option("-u,--user", user, "P4USER");
    app.add_option("-p,--port", port, "P4PORT");
    app.add_option("-c,--client", client, "P4CLIENT");

    p4_cli::register_commands(app);

    // Allow extra arguments for legacy command handling
    app.allow_extras();

    CLI11_PARSE(app, argc, argv);

    // Handle legacy commands if no subcommand was used
    if (app.get_subcommands().size() == 0 && app.remaining().size() > 0) {
        if (!P5::InitializeLibraries()) {
            return 1;
        }

        bool ok = P5().TestConnection().IsError();
        if (ok) {
            INFO("Perforce server is available");
        } else {
            ERROR("Error occurred while connecting to " << P5::P4PORT);
            return 1;
        }

        P5 p5;

        auto remaining = app.remaining();
        if (!remaining.empty()) {
            std::string command = remaining[0];
            std::vector<std::string> args(remaining.begin() + 1, remaining.end());

            dbg(command);
            for (const auto &arg : args) {
                dbg(arg);
            }

            p4_cli::run_forwarded(p5, command.c_str(), args);
        }

        if (!P5::ShutdownLibraries()) {
            return 1;
        }
    }

    return 0;
}
