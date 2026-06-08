#include "reconcile/engine.h"

#include "log.h"
#include "options.h"
#include "reconcile/batch_p4.h"
#include "reconcile/digest.h"
#include "commands/fstat.h"
#include "commands/have.h"
#include "reconcile/workspace_scan.h"
#include "utils/std_helper.h"

#include <cstdio>
#include <iostream>
#include <sstream>
#include <unordered_map>

#ifdef _WIN32
#define popen _popen
#define pclose _pclose
#endif

namespace reconcile {

using p5::DepotFileRecord;
using p5::DepotState;
using p5::DigestType;
using p5::DigestTypeFor;
using p5::FileAction;
using p5::FileType;
using p5::HaveRecord;
using p5::WorkspaceCache;
using p5::WorkspaceFile;
using p5::WorkspaceState;

namespace {

bool IsDeletedHead(FileAction a) {
    return a == FileAction::Delete || a == FileAction::MoveDelete;
}

bool IsAddAction(FileAction a) {
    return a == FileAction::Add || a == FileAction::MoveAdd || a == FileAction::Branch;
}

void PartitionUnchanged(std::vector<DigestWorkItem> &items, const std::unordered_map<std::string, HaveRecord> &have, bool mtimeOptimize, std::vector<DigestWorkItem> &needsDigest, std::vector<const WorkspaceFile *> &unchanged) {
    if (!mtimeOptimize) {
        needsDigest = std::move(items);
        return;
    }
    for (auto &item : items) {
        if (IsUnchangedSinceSync(*item.first, have)) {
            unchanged.push_back(item.first);
        } else {
            needsDigest.push_back(item);
        }
    }
}

} // namespace

AnalyzeResult Analyze(P4API &p4api, const std::string &workDir, const std::vector<std::string> &paths, const ReconcileOptions &opts, WorkspaceCache &cache) {
    AnalyzeResult result;
    ReconcilePlan &plan = result.plan;

    std::vector<std::string> fstatPaths = paths;
    if (fstatPaths.empty()) {
        fstatPaths.push_back("./...");
    }

    result.depot = Fstat::Load(p4api, fstatPaths);
    DepotState &depot = result.depot;
    WorkspaceState workspace = ScanWorkspace(p4api, workDir, opts.skipIgnore);

    std::unordered_map<std::string, HaveRecord> haveRecords;
    if (opts.mtimeOptimize) {
        haveRecords = Have::Load(p4api, fstatPaths);
    }

    std::vector<std::string> doAdd;
    std::vector<std::string> doEdit;
    std::vector<std::string> doReopenEdit;
    std::vector<std::string> doDelete;
    std::vector<std::string> doReopenDelete;
    std::vector<std::string> doRevertAdd;
    std::vector<std::string> doRevertEdit;
    std::vector<std::string> doRevertDelete;

    std::vector<DigestWorkItem> checkEdit;
    std::vector<DigestWorkItem> checkRevertEdit;
    std::vector<DigestWorkItem> checkRevertDeleteOrReopen;

    // Phase 1: depot records
    for (const auto &record : depot.fileRecords) {
        if (!record.headAction) {
            if (record.action && IsAddAction(*record.action)) {
                if (!workspace.hasFiltered(record.clientFileLower)) {
                    doRevertAdd.push_back(record.clientFile);
                }
            }
            continue;
        }

        if (IsDeletedHead(*record.headAction)) {
            continue;
        }

        if (!record.haveRev) {
            continue;
        }

        const FileAction openAction = record.action.value_or(FileAction::Unknown);

        if (openAction == FileAction::Delete || openAction == FileAction::MoveDelete) {
            if (const WorkspaceFile *file = workspace.getFiltered(record.clientFileLower)) {
                if (!record.headType || !record.fileSize) {
                    continue;
                }
                bool supported = true;
                const DigestType dtype = DigestTypeFor(*record.headType, supported);
                if (!supported) {
                    plan.unsupportedDepotFiles.push_back(record.depotFile);
                    continue;
                }
                if (*record.fileSize == file->size && *record.headType != FileType::Binary) {
                    checkRevertDeleteOrReopen.emplace_back(file, dtype);
                } else {
                    doReopenEdit.push_back(record.clientFile);
                }
            }
        } else if (openAction == FileAction::Edit || openAction == FileAction::Integrate) {
            if (!workspace.hasFile(record.clientFileLower)) {
                doReopenDelete.push_back(record.clientFile);
            }
        } else if (openAction == FileAction::Unknown) {
            if (!workspace.hasFile(record.clientFileLower)) {
                doDelete.push_back(record.clientFile);
            }
        }
    }

    // Phase 2: workspace files
    for (const auto &file : workspace.files) {
        if (file.filtered) {
            continue;
        }

        const DepotFileRecord *record = depot.getByClientLower(file.pathLower);
        if (!record) {
            doAdd.push_back(file.path);
            continue;
        }

        if (record->headType) {
            const FileType ft = *record->headType;
            if (ft == FileType::Apple || ft == FileType::Resource) {
                plan.unsupportedDepotFiles.push_back(record->depotFile);
                continue;
            }
        }

        if (!record->headAction) {
            continue;
        }

        if (IsDeletedHead(*record->headAction)) {
            const FileAction openAction = record->action.value_or(FileAction::Unknown);
            if (openAction == FileAction::Unknown) {
                doAdd.push_back(file.path);
            }
            continue;
        }

        const FileAction openAction = record->action.value_or(FileAction::Unknown);
        if (openAction == FileAction::Integrate) {
            continue;
        }
        if (openAction == FileAction::Delete || openAction == FileAction::MoveDelete) {
            continue;
        }

        if (!record->headType || !record->fileSize) {
            continue;
        }

        bool supported = true;
        const DigestType dtype = DigestTypeFor(*record->headType, supported);
        if (!supported) {
            plan.unsupportedDepotFiles.push_back(record->depotFile);
            continue;
        }

        if (openAction == FileAction::Edit) {
            if (*record->fileSize == file.size && *record->headType != FileType::Binary) {
                checkRevertEdit.emplace_back(&file, dtype);
            }
        } else if (openAction == FileAction::Unknown) {
            if (*record->fileSize != file.size && *record->headType == FileType::Binary) {
                doEdit.push_back(file.path);
            } else {
                checkEdit.emplace_back(&file, dtype);
            }
        }
    }

    std::vector<const WorkspaceFile *> revertEditUnchanged;
    std::vector<const WorkspaceFile *> revertDeleteUnchanged;

    std::vector<DigestWorkItem> checkEditDigest;
    std::vector<DigestWorkItem> checkRevertEditDigest;
    std::vector<DigestWorkItem> checkRevertDeleteDigest;

    if (opts.mtimeOptimize) {
        std::vector<const WorkspaceFile *> unused;
        PartitionUnchanged(checkEdit, haveRecords, true, checkEditDigest, unused);
        PartitionUnchanged(checkRevertEdit, haveRecords, true, checkRevertEditDigest, revertEditUnchanged);
        PartitionUnchanged(checkRevertDeleteOrReopen, haveRecords, true, checkRevertDeleteDigest, revertDeleteUnchanged);
    } else {
        checkEditDigest = std::move(checkEdit);
        checkRevertEditDigest = std::move(checkRevertEdit);
        checkRevertDeleteDigest = std::move(checkRevertDeleteOrReopen);
    }

    for (const WorkspaceFile *file : revertEditUnchanged) {
        doRevertEdit.push_back(file->path);
    }
    for (const WorkspaceFile *file : revertDeleteUnchanged) {
        doRevertDelete.push_back(file->path);
    }

    auto processDigest = [&](const std::vector<DigestWorkItem> &work, auto onMatch, auto onMismatch) {
        if (work.empty()) {
            return;
        }
        const std::vector<DigestResult> digests = ParallelComputeDigests(work, cache);
        for (size_t i = 0; i < work.size(); ++i) {
            const WorkspaceFile *file = work[i].first;
            const DepotFileRecord *rec = depot.getByClientLower(file->pathLower);
            if (!rec || !rec->digest || !rec->headType) {
                continue;
            }
            const auto &computed = std::get<1>(digests[i]);
            const bool matches = computed == *rec->digest || DigestMatches(*file, *rec->headType, *rec->digest);
            if (matches) {
                onMatch(*file, *rec);
            } else {
                onMismatch(*file, *rec);
            }
        }
    };

    processDigest(
        checkEditDigest,
        [](const WorkspaceFile &, const DepotFileRecord &) {},
        [&](const WorkspaceFile &file, const DepotFileRecord &) { doEdit.push_back(file.path); });

    processDigest(
        checkRevertEditDigest,
        [&](const WorkspaceFile &file, const DepotFileRecord &) { doRevertEdit.push_back(file.path); },
        [](const WorkspaceFile &, const DepotFileRecord &) {});

    processDigest(
        checkRevertDeleteDigest,
        [&](const WorkspaceFile &file, const DepotFileRecord &) { doRevertDelete.push_back(file.path); },
        [&](const WorkspaceFile &, const DepotFileRecord &rec) { doReopenEdit.push_back(rec.clientFile); });

    plan.doAdd = std::move(doAdd);
    plan.doEdit = std::move(doEdit);
    plan.doReopenEdit = std::move(doReopenEdit);
    plan.doDelete = std::move(doDelete);
    plan.doReopenDelete = std::move(doReopenDelete);
    plan.doRevertAdd = std::move(doRevertAdd);
    plan.doRevertEdit = std::move(doRevertEdit);
    plan.doRevertDelete = std::move(doRevertDelete);

    return result;
}

namespace {

void PrintAction(const std::string &depotFile, int rev, const char *message) {
    if (rev > 0) {
        std::cout << depotFile << '#' << rev << " - " << message << '\n';
    } else {
        std::cout << depotFile << " - " << message << '\n';
    }
}

std::string DepotPathForClientFile(const std::string &clientPath) {
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
    cmd << " where " << ShellQuote(clientPath);

    FILE *pipe = popen(cmd.str().c_str(), "r");
    if (!pipe) {
        return {};
    }

    char buffer[4096];
    std::string line;
    if (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        line = buffer;
    }
    pclose(pipe);

    if (line.empty()) {
        return {};
    }

    const size_t space = line.find(' ');
    if (space == std::string::npos) {
        return {};
    }
    return line.substr(0, space);
}

const DepotFileRecord *FindByClientPath(const DepotState &depot, const std::string &clientPath) {
    return depot.getByClientLower(ToLower(clientPath));
}

} // namespace

void ApplyPlan(P4API &p4api, const ReconcilePlan &plan, const DepotState &depot, const ReconcileOptions &opts) {
    if (!plan.unsupportedDepotFiles.empty()) {
        std::vector<std::string> recArgs;
        if (opts.preview) {
            recArgs.push_back("-n");
        }
        if (opts.changelist != 0) {
            recArgs.push_back("-c");
            recArgs.push_back(std::to_string(opts.changelist));
        }
        recArgs.insert(recArgs.end(), plan.unsupportedDepotFiles.begin(), plan.unsupportedDepotFiles.end());
        p4api.Run("reconcile", recArgs);
    }

    const bool useCl = opts.changelist != 0;

    auto preview = [&](const std::vector<std::string> &files, const char *msg) {
        for (const auto &path : files) {
            const DepotFileRecord *rec = FindByClientPath(depot, path);
            std::string depotPath = rec ? rec->depotFile : DepotPathForClientFile(path);
            if (depotPath.empty()) {
                continue;
            }
            const int rev = rec && rec->haveRev ? *rec->haveRev : 1;
            std::cout << depotPath << '#' << rev << " - " << msg << '\n';
        }
    };

    if (opts.preview) {
        if (opts.add) {
            preview(plan.doAdd, "opened for add");
        }
        if (opts.edit) {
            preview(plan.doEdit, "opened for edit");
            preview(plan.doReopenEdit, "reopened for edit");
        }
        if (opts.deleteFiles) {
            preview(plan.doDelete, "opened for delete");
            preview(plan.doReopenDelete, "reopened for delete");
        }
        preview(plan.doRevertAdd, "reverted");
        preview(plan.doRevertEdit, "reverted");
        preview(plan.doRevertDelete, "reverted");
        return;
    }

    if (opts.add && !plan.doAdd.empty()) {
        RunBatched(p4api, {"add"}, plan.doAdd, useCl, opts.changelist);
        for (const auto &path : plan.doAdd) {
            if (const DepotFileRecord *rec = FindByClientPath(depot, path)) {
                PrintAction(rec->depotFile, rec->haveRev.value_or(1), "opened for add");
            }
        }
    }

    if (opts.edit && !plan.doEdit.empty()) {
        RunBatched(p4api, {"edit"}, plan.doEdit, useCl, opts.changelist);
        for (const auto &path : plan.doEdit) {
            if (const DepotFileRecord *rec = FindByClientPath(depot, path)) {
                PrintAction(rec->depotFile, rec->haveRev.value_or(1), "opened for edit");
            }
        }
    }

    if (opts.edit && !plan.doReopenEdit.empty()) {
        RunBatched(p4api, {"revert", "-k"}, plan.doReopenEdit, false, 0);
        RunBatched(p4api, {"edit"}, plan.doReopenEdit, useCl, opts.changelist);
        for (const auto &path : plan.doReopenEdit) {
            if (const DepotFileRecord *rec = FindByClientPath(depot, path)) {
                PrintAction(rec->depotFile, rec->haveRev.value_or(1), "opened for edit");
            }
        }
    }

    if (opts.deleteFiles && !plan.doDelete.empty()) {
        RunBatched(p4api, {"delete", "-k"}, plan.doDelete, useCl, opts.changelist);
        for (const auto &path : plan.doDelete) {
            if (const DepotFileRecord *rec = FindByClientPath(depot, path)) {
                PrintAction(rec->depotFile, rec->haveRev.value_or(1), "opened for delete");
            }
        }
    }

    if (opts.deleteFiles && !plan.doReopenDelete.empty()) {
        RunBatched(p4api, {"revert", "-k"}, plan.doReopenDelete, false, 0);
        RunBatched(p4api, {"delete", "-k"}, plan.doReopenDelete, useCl, opts.changelist);
        for (const auto &path : plan.doReopenDelete) {
            if (const DepotFileRecord *rec = FindByClientPath(depot, path)) {
                PrintAction(rec->depotFile, rec->haveRev.value_or(1), "opened for delete");
            }
        }
    }

    if (!plan.doRevertAdd.empty()) {
        RunBatched(p4api, {"revert", "-k"}, plan.doRevertAdd, false, 0);
    }
    if (!plan.doRevertEdit.empty()) {
        RunBatched(p4api, {"revert", "-k"}, plan.doRevertEdit, false, 0);
    }
    if (!plan.doRevertDelete.empty()) {
        RunBatched(p4api, {"revert", "-k"}, plan.doRevertDelete, false, 0);
    }
}

} // namespace reconcile
