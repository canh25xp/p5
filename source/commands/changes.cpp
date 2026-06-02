#include <CLI/CLI.hpp>

#include "commands/changes.h"
#include "commands.h"
#include "p5.h"
#include "options.h"

#include <p4/clientapi.h>

#include "log.h"
#include "utils/text_width.h"

#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <sstream>
#include <vector>

namespace {

std::string FormatChangeTime(const std::string &unixTime, bool includeTime) {
    if (unixTime.empty()) {
        return {};
    }
    char *end = nullptr;
    long ts = std::strtol(unixTime.c_str(), &end, 10);
    if (end == unixTime.c_str() || *end != '\0') {
        return unixTime;
    }
    std::time_t t = static_cast<std::time_t>(ts);
    std::tm tm_buf{};
#if defined(_WIN32)
    localtime_s(&tm_buf, &t);
#else
    localtime_r(&t, &tm_buf);
#endif
    char buf[64];
    if (includeTime) {
        if (std::strftime(buf, sizeof(buf), "%Y/%m/%d %H:%M:%S", &tm_buf) == 0) {
            return unixTime;
        }
    } else if (std::strftime(buf, sizeof(buf), "%Y/%m/%d", &tm_buf) == 0) {
        return unixTime;
    }
    return buf;
}

void TruncateDescription(std::string &desc, size_t maxLen) {
    if (desc.size() > maxLen) {
        desc.resize(maxLen);
    }
}

} // namespace

void Changes::OutputStat(StrDict *varList) {
    StrPtr *changePtr = varList->GetVar("change");
    if (!changePtr) {
        ERROR("Changelist number not found in output");
        return;
    }

    ChangeNumber changeNum = std::atoi(changePtr->Text());
    ChangeData data;

    StrPtr *timePtr = varList->GetVar("time");
    if (timePtr) {
        data.time = timePtr->Text();
    }

    StrPtr *userPtr = varList->GetVar("user");
    if (userPtr) {
        data.user = userPtr->Text();
    }

    StrPtr *clientPtr = varList->GetVar("client");
    if (clientPtr) {
        data.client = clientPtr->Text();
    }

    StrPtr *statusPtr = varList->GetVar("status");
    if (statusPtr) {
        data.status = statusPtr->Text();
    }

    StrPtr *descPtr = varList->GetVar("desc");
    if (descPtr) {
        data.description = descPtr->Text();
    }

    m_order.push_back(changeNum);
    m_changes.push_back(std::move(data));
}

void Changes::PrintSortedTable(std::ostream &out, bool includeTime, bool truncateDescription, bool reverse) const {
    if (m_order.empty()) {
        return;
    }

    std::vector<size_t> indices(m_order.size());
    for (size_t i = 0; i < indices.size(); ++i) {
        indices[i] = i;
    }
    if (!reverse) {
        std::sort(indices.begin(), indices.end(), [this](size_t a, size_t b) {
            return m_order[a] < m_order[b];
        });
    }

    size_t maxChangeWidth = 6;
    size_t maxDateWidth = 4;
    size_t maxUserWidth = 4;
    size_t maxClientWidth = 6;
    size_t maxStatusWidth = 6;
    size_t maxDescWidth = 11;

    std::vector<std::string> dates;
    dates.reserve(indices.size());
    for (size_t idx : indices) {
        const ChangeData &data = m_changes[idx];
        std::string date = FormatChangeTime(data.time, includeTime);
        dates.push_back(date);

        std::string changeStr = std::to_string(m_order[idx]);
        maxChangeWidth = std::max(maxChangeWidth, p5::DisplayWidth(changeStr));
        maxDateWidth = std::max(maxDateWidth, p5::DisplayWidth(date));
        maxUserWidth = std::max(maxUserWidth, p5::DisplayWidth(data.user));
        maxClientWidth = std::max(maxClientWidth, p5::DisplayWidth(data.client));
        maxStatusWidth = std::max(maxStatusWidth, p5::DisplayWidth(data.status));

        std::string desc = data.description;
        if (truncateDescription) {
            TruncateDescription(desc, 250);
        }
        maxDescWidth = std::max(maxDescWidth, p5::DisplayWidth(desc));
    }

    auto pad = [](size_t width, const std::string &text) {
        return std::string(width - p5::DisplayWidth(text) + 2, ' ');
    };

    out << "Change" << pad(maxChangeWidth, "Change") << "Date" << pad(maxDateWidth, "Date") << "User" << pad(maxUserWidth, "User")
        << "Client" << pad(maxClientWidth, "Client") << "Status" << pad(maxStatusWidth, "Status") << "Description" << '\n';

    for (size_t i = 0; i < indices.size(); ++i) {
        const size_t idx = indices[i];
        const ChangeData &data = m_changes[idx];
        std::string changeStr = std::to_string(m_order[idx]);
        std::string desc = data.description;
        if (truncateDescription) {
            TruncateDescription(desc, 250);
        }

        out << changeStr << pad(maxChangeWidth, changeStr) << dates[i] << pad(maxDateWidth, dates[i]) << data.user
            << pad(maxUserWidth, data.user) << data.client << pad(maxClientWidth, data.client) << data.status
            << pad(maxStatusWidth, data.status) << desc << '\n';
    }
}

void Changes::PrintFormatted(std::ostream &out, bool includeTime, bool reverse) const {
    std::vector<size_t> indices(m_order.size());
    for (size_t i = 0; i < indices.size(); ++i) {
        indices[i] = i;
    }
    if (!reverse) {
        std::sort(indices.begin(), indices.end(), [this](size_t a, size_t b) {
            return m_order[a] < m_order[b];
        });
    }

    for (size_t idx : indices) {
        const ChangeData &data = m_changes[idx];
        const std::string date = FormatChangeTime(data.time, includeTime);
        out << "Change " << m_order[idx] << " on " << date << " by " << data.user << '@' << data.client << '\n';
        if (!data.description.empty()) {
            out << '\n';
            std::istringstream lines(data.description);
            std::string line;
            while (std::getline(lines, line)) {
                out << '\t' << line << '\n';
            }
            out << '\n';
        }
    }
}

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
    if (m_me) {
        args.push_back("-u");
        args.push_back(g_options.user());
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

    Changes r = p5.RunChanges(args);
    if (r.IsError()) {
        throw CLI::RuntimeError(1);
    }

    if (m_long) {
        r.PrintFormatted(std::cout, m_time, m_reverse);
    } else {
        r.PrintSortedTable(std::cout, m_time, m_longTrunc, m_reverse);
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
    sub->add_flag("--me", m_me, "Display only changes owned by the current user");
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
