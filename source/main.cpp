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

    CLI11_PARSE(app, argc, argv);

    dbg(user, port, client);

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

    const char *command = argv[1];
    int argumentCount = argc - 2;
    char **arguments = argv + 2;

    dbg(command);
    dbg(argumentCount);

    for (size_t i = 0; i < argumentCount; i++) {
        dbg(arguments[i]);
    }

    p5.RunCmd(command, argumentCount, arguments);

    if (!P5::ShutdownLibraries()) {
        return 1;
    }

    return 0;
}
