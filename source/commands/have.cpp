#include <CLI/CLI.hpp>

#include "commands/have.h"
#include "commands.h"
#include "log.h"
#include "p5.h"
#include "options.h"

#include <p4/clientapi.h>

#include <cstdlib>
#include <cstring>

namespace {

using p5::HaveRecord;

std::string ToLower(std::string s) {
    for (char &c : s) {
        if (c >= 'A' && c <= 'Z') {
            c = static_cast<char>(c - 'A' + 'a');
        }
    }
    return s;
}

} // namespace

void Have::OutputStat(StrDict *varList) {
    if (!varList) {
        return;
    }

    StrPtr *code = varList->GetVar("code");
    if (code && std::strcmp(code->Text(), "stat") != 0) {
        return;
    }

    StrPtr *path = varList->GetVar("path");
    if (!path) {
        return;
    }

    HaveRecord rec;
    rec.path = path->Text();
    if (StrPtr *depotFile = varList->GetVar("depotFile")) {
        rec.depotFile = depotFile->Text();
    }
    if (StrPtr *clientFile = varList->GetVar("clientFile")) {
        rec.clientFile = clientFile->Text();
    }
    if (StrPtr *haveRev = varList->GetVar("haveRev")) {
        rec.haveRev = std::atoi(haveRev->Text());
    }
    if (StrPtr *syncTime = varList->GetVar("syncTime")) {
        rec.syncTime = std::strtoll(syncTime->Text(), nullptr, 10);
    }

    m_records[ToLower(rec.path)] = rec;
}

std::unordered_map<std::string, HaveRecord> Have::Load(P5 &p5, const std::vector<std::string> &paths) {
    std::vector<std::string> args = paths;
    if (args.empty()) {
        args.push_back("...");
    }

    return p5.RunHave(args).records();
}

void Have::run(const std::vector<std::string> &args) {
    g_options.set_command(name);
    P5 &p5 = m_commands->p5();

    Have r = p5.RunHave(args);
    if (r.IsError()) {
        CLI_ERROR("have command failed");
        std::exit(1);
    }

    for (const auto &[_, rec] : r.records()) {
        PRINT(rec.depotFile << (rec.haveRev ? ("#" + std::to_string(*rec.haveRev)) : "") << " - " << rec.clientFile);
    }
}

void Have::register_cli(CLI::App &app) {
    auto *sub = app.add_subcommand(name, description);
    sub->prefix_command();

    sub->callback([this, sub]() {
        this->run(sub->remaining());
    });
}
