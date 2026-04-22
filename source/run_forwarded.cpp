#include "run_forwarded.h"

#include <vector>

#include "CLI/CLI.hpp"
#include "dbg.h"

#include "p5.h"

void run_forwarded(P5 &p5, const char *command, const std::vector<std::string> &args) {
    std::vector<char *> argv_array;
    argv_array.reserve(args.size());
    for (const auto &arg : args) {
        argv_array.push_back(const_cast<char *>(arg.c_str()));
    }

    char **argv_ptr = argv_array.empty() ? nullptr : argv_array.data();
    p5.Run(command, static_cast<int>(args.size()), argv_ptr);
}

void run_p4_passthrough(const char *command, const std::vector<std::string> &args) {
    P5 p5;

    dbg(command);
    for (const auto &arg : args) {
        dbg(arg);
    }

    run_forwarded(p5, command, args);
}
