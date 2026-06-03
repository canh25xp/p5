#include <CLI/CLI.hpp>

#include "commands/filelog.h"
#include "commands.h"
#include "log.h"
#include "p5.h"
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

std::vector<p5::FilelogFile> Filelog::Load(P5 &p5, const std::vector<std::string> &args) {
    return p5.RunFilelog(args).files();
}

void Filelog::run(const std::vector<std::string> &args) {
    g_options.set_command(name);
    P5 &p5 = m_commands->p5();

    Filelog result = p5.RunFilelog(args);
    if (result.IsError()) {
        CLI_ERROR("filelog command failed");
        std::exit(1);
    }

    for (const p5::FilelogFile &file : result.files()) {
        std::cout << p5::FormatFilelog(file) << '\n';
    }
}

void Filelog::register_cli(CLI::App &app) {
    auto *sub = app.add_subcommand(name, description);
    sub->prefix_command();

    sub->callback([this, sub]() {
        this->run(sub->remaining());
    });
}
