#include <CLI/CLI.hpp>

#include "commands/filelog.h"
#include "commands.h"
#include "log.h"
#include "p4api.h"
#include "options.h"

#include <p4/clientapi.h>

#include <cstdlib>
#include <cstring>
#include <iostream>

void Filelog::OutputStat(StrDict *varList) {
    if (!varList) {
        return;
    }

    StrPtr *code = varList->GetVar("code");
    if (code && std::strcmp(code->Text(), "stat") != 0) {
        return;
    }

    try {
        m_files.push_back(p5::ParseFilelogStat(varList));
    } catch (const std::exception &ex) {
        ERROR("Failed to parse filelog stat: " << ex.what());
    }
}

std::vector<p5::FilelogFile> Filelog::Load(P4API &p4api, const std::vector<std::string> &args) {
    return p4api.RunFilelog(args).files();
}

std::vector<std::string> Filelog::buildP4Args(const std::vector<std::string> &files) const {
    std::vector<std::string> args;

    if (m_changelist > 0) {
        args.push_back("-c");
        args.push_back(std::to_string(m_changelist));
    }
    if (m_contentHistory) {
        args.push_back("-h");
    }
    if (m_inherited) {
        args.push_back("-i");
    }
    if (m_long) {
        args.push_back("-l");
    }
    if (m_longTrunc) {
        args.push_back("-L");
    }
    if (m_time) {
        args.push_back("-t");
    }
    if (m_max > 0) {
        args.push_back("-m");
        args.push_back(std::to_string(m_max));
    }
    if (m_noPromote) {
        args.push_back("-p");
    }
    if (m_short) {
        args.push_back("-s");
    }

    args.insert(args.end(), files.begin(), files.end());
    return args;
}

void Filelog::run(const std::vector<std::string> &args) {
    g_options.set_command(name);
    P4API &p4api = m_commands->p4api();

    Filelog result = p4api.RunFilelog(args);
    if (result.IsError()) {
        CLI_ERROR("filelog command failed");
    }

    p5::FilelogFormatOptions opts;
    opts.includeTime = m_time;
    opts.longDesc = m_long;
    opts.truncDesc = m_longTrunc;

    for (const p5::FilelogFile &file : result.files()) {
        std::cout << p5::FormatFilelog(file, opts) << '\n';
    }
}

void Filelog::register_cli(CLI::App &app) {
    auto *sub = app.add_subcommand(name, description);

    // p4 filelog uses -h for content history; free the short name from inherited --help.
    sub->set_help_flag("--help");

    sub->add_option("-c", m_changelist, "Display files submitted at the specified changelist number");
    sub->add_flag("-h", m_contentHistory, "Display file content history instead of file name history");
    sub->add_flag("-i", m_inherited, "Include inherited file history from integrate branches");
    sub->add_flag("-l", m_long, "Display the full text of changelist descriptions");
    sub->add_flag("-L", m_longTrunc, "Display changelist descriptions truncated to 250 characters");
    sub->add_flag("-t,--time", m_time, "Display the time as well as the date");
    sub->add_option("-m,--max", m_max, "Display at most this many revisions per file");
    sub->add_flag("-p", m_noPromote, "With -h, do not follow content of promoted task streams");
    sub->add_flag("-s", m_short, "Display shortened filelog omitting non-contributory integrations");

    sub->allow_extras();

    sub->callback([this, sub]() {
        if (m_noPromote && !m_contentHistory) {
            CLI_ERROR("The -p flag requires -h");
        }
        this->run(buildP4Args(sub->remaining()));
    });
}
