#include "run_forwarded.h"

#include <utility>
#include <vector>

#include "CLI/CLI.hpp"
#include "dbg.h"

#include "p5.h"

void apply_connection_options(const std::string &user, const std::string &port, const std::string &client) {
    P5::P4USER = user;
    P5::P4PORT = port;
    P5::P4CLIENT = client;
}

void apply_protocol_options(const std::vector<std::string> &protocol_z) {
    P5::P4PROTOCOL_Z.clear();
    P5::P4PROTOCOL_Z.reserve(protocol_z.size());
    for (const std::string &entry : protocol_z) {
        const std::string::size_type eq = entry.find('=');
        std::string key = eq == std::string::npos ? entry : entry.substr(0, eq);
        std::string value = eq == std::string::npos ? std::string() : entry.substr(eq + 1);
        if (!key.empty()) {
            P5::P4PROTOCOL_Z.emplace_back(std::move(key), std::move(value));
        }
    }
}

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
