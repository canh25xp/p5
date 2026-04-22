#include "CLI/CLI.hpp"

#include "p5.h"
#include "run_forwarded.h"

#ifndef P5_APP_DESCRIPTION
#define P5_APP_DESCRIPTION "p5"
#endif

int main(int argc, char **argv);
int main(int argc, char **argv) {
    CLI::App app(P5_APP_DESCRIPTION);

    argv = app.ensure_utf8(argv);

    P5 cli(P5ForCliConfig{});
    cli.register_cli(app);

    CLI11_PARSE(app, argc, argv);

    return 0;
}
