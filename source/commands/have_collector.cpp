#include "commands/have_collector.h"

#include "p5.h"

#include <p4/clientapi.h>

#include <cstdlib>
#include <cstring>

namespace {

using reconcile::HaveRecord;

std::string ToLower(std::string s) {
    for (char &c : s) {
        if (c >= 'A' && c <= 'Z') {
            c = static_cast<char>(c - 'A' + 'a');
        }
    }
    return s;
}

} // namespace

void HaveCollector::OutputStat(StrDict *varList) {
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
    if (StrPtr *syncTime = varList->GetVar("syncTime")) {
        rec.syncTime = std::strtoll(syncTime->Text(), nullptr, 10);
    }

    m_records[ToLower(path->Text())] = rec;
}

std::unordered_map<std::string, HaveRecord> HaveCollector::Load(P5 &p5, const std::vector<std::string> &paths) {
    std::vector<std::string> args = paths;
    if (args.empty()) {
        args.push_back("...");
    }

    return p5.RunHave(args).records();
}
