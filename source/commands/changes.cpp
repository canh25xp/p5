#include <CLI/CLI.hpp>

#include "commands/changes.h"
#include "commands.h"
#include "p5.h"
#include "options.h"

#include "log.h"

#include <iostream>
#include <vector>

std::vector<std::string> Changes::buildP4Args(const std::vector<std::string> &files) const {
    std::vector<std::string> args;

    if (m_integrated) {
        args.push_back("-i");
    }
    if (m_time) {
        args.push_back("-t");
    }
    if (m_long) {
        args.push_back("-l");
    }
    if (m_longTrunc) {
        args.push_back("-L");
    }
    if (m_force) {
        args.push_back("-f");
    }
    if (m_reverse) {
        args.push_back("-r");
    }
    for (const std::string &client : m_client) {
        args.push_back("-c");
        args.push_back(client);
    }
    if (m_clientCaseInsensitive && !m_client.empty()) {
        args.push_back("-E");
    }
    if (m_afterChange > 0) {
        args.push_back("-e");
        args.push_back(std::to_string(m_afterChange));
    }
    if (m_max > 0) {
        args.push_back("-m");
        args.push_back(std::to_string(m_max));
    }
    if (!m_status.empty()) {
        args.push_back("-s");
        args.push_back(m_status);
    }
    for (const std::string &user : m_user) {
        args.push_back("-u");
        args.push_back(user);
    }
    if (m_userCaseInsensitive && !m_user.empty()) {
        args.push_back("-E");
    }
    if (m_stream) {
        args.push_back("--stream");
    }
    if (m_nostream) {
        args.push_back("--nostream");
    }

    args.insert(args.end(), files.begin(), files.end());
    return args;
}

void Changes::run(const std::vector<std::string> &args) {
    g_options.set_command(name);
    P5 &p5 = m_commands->p5();

    Result r = p5.Run("changes", args);
    if (r.IsError()) {
        throw CLI::RuntimeError(1);
    }
}

void Changes::register_cli(CLI::App &app) {
    auto *sub = app.add_subcommand(name, description);

    for (const std::string &alias : aliases) {
        sub->alias(alias);
    }

    sub->add_flag("-i", m_integrated, "Include changelists integrated into the specified files");
    sub->add_flag("-t,--time", m_time, "Display the time as well as the date");
    sub->add_flag("-l", m_long, "Display the full text of changelist descriptions");
    sub->add_flag("-L", m_longTrunc, "Display changelist descriptions truncated to 250 characters");
    sub->add_flag("-f", m_force, "Enable admin users to view restricted changes");
    sub->add_flag("-r", m_reverse, "Sort output in reverse order");
    sub->add_option("-c,--client", m_client, "Display only changes owned by the specified client")->expected(0, -1);
    sub->add_flag("-E,--client-case-insensitive", m_clientCaseInsensitive,
        "Treat -c client patterns as case-insensitive (p4 -E with -c)");
    sub->add_option("-e", m_afterChange, "Display only changes at or above this changelist number");
    sub->add_option("-m,--max", m_max, "Limit output to the specified number of changelists");
    sub->add_option("-s,--status", m_status, "Limit output to changelists with the specified status")
        ->check(CLI::IsMember({"pending", "shelved", "submitted"}));
    sub->add_option("-u,--user", m_user, "Display only changes owned by the specified user")->expected(0, -1);
    sub->add_flag("--user-case-insensitive", m_userCaseInsensitive,
        "Treat -u user patterns as case-insensitive (p4 -E with -u)");
    sub->add_flag("--stream", m_stream, "Display only changes that contain a stream spec");
    sub->add_flag("--nostream", m_nostream, "Display only changes that do not contain a stream spec");

    sub->callback([this, sub]() {
        std::vector<std::string> args = buildP4Args(sub->remaining());
        this->run(args);
    });
}
