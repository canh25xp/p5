#include <string>
#include <vector>

#include "CLI/CLI.hpp"
#include "dbg.h"

#include "log.h"
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

    // Add new placeholder subcommand
    auto new_subcommand = app.add_subcommand("new-command", "A new placeholder subcommand");
    new_subcommand->callback([]() {
        std::cout << "This is a placeholder for the new subcommand." << std::endl;
        std::cout << "Implementation to be added later." << std::endl;
    });

    // Legacy command handling for backward compatibility
    app.allow_extras();
    app.callback([&]() {
        auto extras = app.remaining();
        if (!extras.empty()) {
            if (!P5::InitializeLibraries()) {
                return;
            }

            bool ok = P5().TestConnection().IsError();
            if (ok) {
                INFO("Perforce server is available");
            } else {
                ERROR("Error occurred while connecting to " << P5::P4PORT);
                return;
            }

            P5 p5;

            // Convert remaining args to traditional argv format
            std::vector<std::string> args = extras;
            std::vector<char*> argv_array;
            for (auto& arg : args) {
                argv_array.push_back(const_cast<char*>(arg.c_str()));
            }

            if (!argv_array.empty()) {
                const char *command = argv_array[0];
                int argumentCount = argv_array.size() - 1;
                char **arguments = argv_array.data() + 1;

                dbg(command);
                dbg(argumentCount);

                for (size_t i = 0; i < argumentCount; i++) {
                    dbg(arguments[i]);
                }

                p5.RunCmd(command, argumentCount, arguments);
            }

            if (!P5::ShutdownLibraries()) {
                return;
            }
        }
    });

    CLI11_PARSE(app, argc, argv);

    // Handle subcommands that don't need P5 initialization
    if (new_subcommand->parsed()) {
        return 0;
    }

    return 0;
}
