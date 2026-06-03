#include <CLI/CLI.hpp>

#include "commands/fstat.h"
#include "commands.h"
#include "log.h"
#include "p5.h"
#include "options.h"

#include <p4/clientapi.h>

#include <cstdlib>
#include <cstring>

namespace {

using p5::DepotFileRecord;
using p5::DepotState;
using p5::ParseFileAction;
using p5::ParseFileType;

std::string ToLower(std::string s) {
    for (char &c : s) {
        if (c >= 'A' && c <= 'Z') {
            c = static_cast<char>(c - 'A' + 'a');
        }
    }
    return s;
}

bool ParseDigestHex(const char *hex, std::array<uint8_t, 16> &out) {
    if (!hex || std::strlen(hex) != 32) {
        return false;
    }
    for (int i = 0; i < 16; ++i) {
        char buf[3] = {hex[i * 2], hex[i * 2 + 1], '\0'};
        char *end = nullptr;
        unsigned long v = std::strtoul(buf, &end, 16);
        if (end != buf + 2) {
            return false;
        }
        out[static_cast<size_t>(i)] = static_cast<uint8_t>(v);
    }
    return true;
}

} // namespace

void Fstat::OutputStat(StrDict *varList) {
    if (!varList) {
        return;
    }

    StrPtr *code = varList->GetVar("code");
    if (code && std::strcmp(code->Text(), "stat") != 0) {
        return;
    }

    DepotFileRecord record;

    if (StrPtr *v = varList->GetVar("depotFile")) {
        record.depotFile = v->Text();
        record.depotFileLower = ToLower(record.depotFile);
    }
    if (StrPtr *v = varList->GetVar("clientFile")) {
        record.clientFile = v->Text();
        record.clientFileLower = ToLower(record.clientFile);
    }
    if (StrPtr *v = varList->GetVar("headType")) {
        record.headType = ParseFileType(v->Text());
    }
    if (StrPtr *v = varList->GetVar("headRev")) {
        record.headRev = std::atoi(v->Text());
    }
    if (StrPtr *v = varList->GetVar("haveRev")) {
        record.haveRev = std::atoi(v->Text());
    }
    if (StrPtr *v = varList->GetVar("headAction")) {
        record.headAction = ParseFileAction(v->Text());
    }
    if (StrPtr *v = varList->GetVar("action")) {
        record.action = ParseFileAction(v->Text());
    }
    if (StrPtr *v = varList->GetVar("fileSize")) {
        record.fileSize = static_cast<uint64_t>(std::strtoull(v->Text(), nullptr, 10));
    }
    if (StrPtr *v = varList->GetVar("digest")) {
        std::array<uint8_t, 16> digest{};
        if (ParseDigestHex(v->Text(), digest)) {
            record.digest = digest;
        }
    }

    if (!record.depotFile.empty()) {
        m_state.fileRecords.push_back(std::move(record));
    }
}

DepotState Fstat::Load(P5 &p5, const std::vector<std::string> &paths) {
    std::vector<std::string> fstatArgs = {
        "-Rc",
        "-Ol",
        "-T",
        "depotFile,clientFile,headAction,headType,headRev,haveRev,digest,fileSize,action",
    };
    fstatArgs.insert(fstatArgs.end(), paths.begin(), paths.end());

    Fstat fstat = p5.RunFstat(fstatArgs);
    fstat.state().buildMapping();

    std::vector<std::string> staleRefs;
    for (const auto &record : fstat.state().fileRecords) {
        if (record.haveRev && record.headRev && *record.haveRev != *record.headRev) {
            staleRefs.push_back(record.depotFile + "#" + std::to_string(*record.haveRev));
        }
    }

    if (!staleRefs.empty()) {
        std::vector<std::string> refreshArgs = {
            "-Rc",
            "-Ol",
            "-T",
            "depotFile,headType,headAction,fileSize,digest",
        };
        refreshArgs.insert(refreshArgs.end(), staleRefs.begin(), staleRefs.end());
        Fstat refresh = p5.RunFstat(refreshArgs);

        for (const auto &refreshed : refresh.state().fileRecords) {
            if (auto *original = fstat.state().getByDepotLower(refreshed.depotFileLower)) {
                if (refreshed.headType) {
                    original->headType = refreshed.headType;
                }
                if (refreshed.headAction) {
                    original->headAction = refreshed.headAction;
                }
                if (refreshed.fileSize) {
                    original->fileSize = refreshed.fileSize;
                }
                if (refreshed.digest) {
                    original->digest = refreshed.digest;
                }
            }
        }
    }

    return fstat.state();
}

void Fstat::run(const std::vector<std::string> &args) {
    g_options.set_command(name);
    P5 &p5 = m_commands->p5();

    Fstat r = p5.RunFstat(args);
    if (r.IsError()) {
        CLI_ERROR("fstat command failed");
        std::exit(1);
    }

    for (const auto &record : r.state().fileRecords) {
        PRINT(record.depotFile << (record.haveRev ? ("#" + std::to_string(*record.haveRev)) : "") << " - " << record.clientFile);
    }
}

void Fstat::register_cli(CLI::App &app) {
    auto *sub = app.add_subcommand(name, description);
    sub->prefix_command();

    sub->callback([this, sub]() {
        this->run(sub->remaining());
    });
}
