#include <CLI/CLI.hpp>

#include "commands/have.h"
#include "commands.h"
#include "log.h"
#include "p4api.h"
#include "options.h"
#include "utils/std_helper.h"

#include <p4/clientapi.h>

#include <cstdlib>
#include <cstring>

using p5::HaveRecord;

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

std::unordered_map<std::string, HaveRecord> Have::Load(P4API &p4api, const std::vector<std::string> &paths) {
    std::vector<std::string> args = paths;
    if (args.empty()) {
        args.push_back("...");
    }

    return p4api.RunHave(args).records();
}

void Have::run(const std::vector<std::string> &args) {
    g_options.set_command(name);
    P4API &p4api = m_commands->p4api();

    Have r = p4api.RunHave(args);
    if (r.IsError()) {
        CLI_ERROR("have command failed");
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
