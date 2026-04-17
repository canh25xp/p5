#include <string>
#include <vector>

#include "CLI/CLI.hpp"

#include "global_options.h"
#include "registry.h"
#include "run_forwarded.h"

int main(int argc, char **argv);
int main(int argc, char **argv) {
    CLI::App app("p5: Complementary p4 command-line tools");

    argv = app.ensure_utf8(argv);

    GlobalOptions connection;
    register_global_options(app, connection);

    register_commands(app);

    // Allow extra arguments for legacy command handling
    app.allow_extras();

    CLI11_PARSE(app, argc, argv);

    // Handle legacy commands if no subcommand was used
    if (app.get_subcommands().size() == 0 && app.remaining().size() > 0) {
        const auto remaining = app.remaining();
        const std::string &command = remaining[0];
        const std::vector<std::string> args(remaining.begin() + 1, remaining.end());
        try {
            run_p4_passthrough(command.c_str(), args);
        } catch (const CLI::RuntimeError &e) {
            return e.get_exit_code();
        }
    }

    return 0;
}
