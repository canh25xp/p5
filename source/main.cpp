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

    // Add example subcommand
    auto example_cmd = app.add_subcommand("example", "Example subcommand - placeholder for future implementation");

    // Example subcommand options
    std::string example_path;
    bool example_verbose = false;

    example_cmd->add_option("path", example_path, "Path to process")->required();
    example_cmd->add_flag("-v,--verbose", example_verbose, "Enable verbose output");

    // Set up callback for example subcommand
    example_cmd->callback([&]() {
        if (!P5::InitializeLibraries()) {
            throw CLI::RuntimeError(1);
        }

        // Test connection
        bool ok = P5().TestConnection().IsError();
        if (ok) {
            INFO("Perforce server is available");
        } else {
            ERROR("Error occurred while connecting to " << P5::P4PORT);
            throw CLI::RuntimeError(1);
        }

        P5 p5;

        // Placeholder implementation
        std::cout << "Example subcommand executed!" << std::endl;
        std::cout << "Path: " << example_path << std::endl;
        std::cout << "Verbose: " << (example_verbose ? "true" : "false") << std::endl;
        std::cout << "TODO: Implement actual functionality here" << std::endl;

        if (!P5::ShutdownLibraries()) {
            throw CLI::RuntimeError(1);
        }
    });

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

        // Get remaining arguments for legacy command handling
        auto remaining = app.remaining();
        if (!remaining.empty()) {
            std::string command = remaining[0];
            std::vector<std::string> args(remaining.begin() + 1, remaining.end());

            // Convert to char* array for Run
            std::vector<char *> argv_array;
            argv_array.push_back(const_cast<char *>(command.c_str()));
            for (const auto &arg : args) {
                argv_array.push_back(const_cast<char *>(arg.c_str()));
            }

            dbg(command);
            for (const auto &arg : args) {
                dbg(arg);
            }

            p5.Run(command.c_str(), static_cast<int>(args.size()), argv_array.data() + 1);
        }

        if (!P5::ShutdownLibraries()) {
            return 1;
        }
    }

    return 0;
}
