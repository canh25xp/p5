#include "CLI/CLI.hpp"

#include "p5.h"

#include <iostream>

void run_users(const std::vector<std::string> &args) {
    if (!P5::InitializeLibraries()) {
        throw CLI::RuntimeError(1);
    }

    bool printDone = false;
    {
        P5 p5;
        UsersResult r = p5.Users(args);
        // Result::IsError() returns true when there is no client-side error.
        if (r.IsError()) {
            r.PrintSortedTsv(std::cout);
            printDone = true;
        }
    }

    if (!P5::ShutdownLibraries()) {
        throw CLI::RuntimeError(1);
    }

    if (!printDone) {
        throw CLI::RuntimeError(1);
    }
}
