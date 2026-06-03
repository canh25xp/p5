#include "reconcile/workspace_scan.h"

#include "commands/result.h"
#include "options.h"
#include "utils/std_helper.h"

#include <cstdio>
#include <filesystem>
#include <sstream>
#include <unordered_set>

namespace reconcile {

using p5::WorkspaceFile;
using p5::WorkspaceState;

namespace fs = std::filesystem;

namespace {

class IgnoreResult : public Result {
public:
    std::unordered_set<std::string> ignored;

    void recordIgnoredLine(const char *text) { // NOLINT(readability-make-member-function-const)
        if (!text) {
            return;
        }
        std::string line(text);
        while (!line.empty() && (line.back() == '\n' || line.back() == '\r')) {
            line.pop_back();
        }
        const std::string suffix = " ignored";
        if (line.size() > suffix.size() && line.compare(line.size() - suffix.size(), suffix.size(), suffix) == 0) {
            ignored.insert(line.substr(0, line.size() - suffix.size()));
        }
    }

    void Message(Error *e) override {
        if (!e) {
            return;
        }
        StrBuf msg;
        e->Fmt(&msg);
        recordIgnoredLine(msg.Text());
    }

    void OutputInfo(char /*level*/, const char *data) override { recordIgnoredLine(data); }

    void HandleError(Error *e) override {
        if (e && e->IsInfo()) {
            StrBuf msg;
            e->Fmt(&msg);
            recordIgnoredLine(msg.Text());
        }
        Result::HandleError(e);
    }
};

namespace {

void RunIgnoresBatch(const std::vector<std::string> &paths, IgnoreResult &ignoreResult) {
    if (paths.empty()) {
        return;
    }

    std::ostringstream cmd;
    cmd << "p4";
    if (!g_options.port().empty()) {
        cmd << " -p " << ShellQuote(g_options.port());
    }
    if (!g_options.user().empty()) {
        cmd << " -u " << ShellQuote(g_options.user());
    }
    if (!g_options.client().empty()) {
        cmd << " -c " << ShellQuote(g_options.client());
    }
    cmd << " ignores -i";
    for (const auto &path : paths) {
        cmd << ' ' << ShellQuote(path);
    }

    FILE *pipe = popen(cmd.str().c_str(), "r");
    if (!pipe) {
        return;
    }

    char buffer[4096];
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        ignoreResult.recordIgnoredLine(buffer);
    }
    pclose(pipe);
}

} // namespace

void ApplyIgnores(P5 & /*p5*/, WorkspaceState &state) {
    if (state.files.empty()) {
        return;
    }

    std::vector<std::string> paths;
    paths.reserve(state.files.size());
    for (const auto &f : state.files) {
        paths.push_back(f.path);
    }

    IgnoreResult ignoreResult;
    const size_t batchSize = 200;
    for (size_t i = 0; i < paths.size(); i += batchSize) {
        const size_t end = std::min(i + batchSize, paths.size());
        RunIgnoresBatch({paths.begin() + static_cast<std::ptrdiff_t>(i), paths.begin() + static_cast<std::ptrdiff_t>(end)}, ignoreResult);
    }

    for (auto &file : state.files) {
        if (ignoreResult.ignored.count(file.path) > 0) {
            file.filtered = true;
        }
    }
}

} // namespace

WorkspaceState ScanWorkspace(P5 &p5, const std::string &rootDir, bool skipIgnoreCheck) {
    WorkspaceState state;
    const fs::path root(rootDir);

    if (!fs::exists(root)) {
        return state;
    }

    std::error_code ec;
    for (const auto &entry : fs::recursive_directory_iterator(root, fs::directory_options::skip_permission_denied, ec)) {
        if (ec || !entry.is_regular_file()) {
            continue;
        }

        std::error_code sizeEc;
        const auto size = entry.file_size(sizeEc);
        if (sizeEc) {
            continue;
        }

        std::error_code timeEc;
        const auto ftime = entry.last_write_time(timeEc);
        if (timeEc) {
            continue;
        }

        const auto sctp = std::chrono::time_point_cast<std::chrono::seconds>(ftime);
        const int64_t mtimeSecs = sctp.time_since_epoch().count();

        WorkspaceFile file;
        file.path = entry.path().string();
        file.pathLower = ToLower(file.path);
        file.size = size;
        file.mtimeSecs = mtimeSecs;
        state.files.push_back(std::move(file));
    }

    if (!skipIgnoreCheck) {
        ApplyIgnores(p5, state);
    }

    size_t active = 0;
    for (const auto &f : state.files) {
        if (!f.filtered) {
            active++;
        }
    }
    state.numFiles = active;
    state.buildMapping();
    return state;
}

} // namespace reconcile
