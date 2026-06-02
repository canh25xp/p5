#include <CLI/CLI.hpp>

#include "commands/fast_reconcile.h"
#include "commands.h"
#include "options.h"
#include "p5.h"
#include "reconcile/digest.h"
#include "reconcile/engine.h"

#include <filesystem>
#include <iostream>
#include <unordered_set>

namespace fs = std::filesystem;

namespace {

const std::unordered_set<std::string> kPassthroughFlags = {
    "-w",
    "-k",
    "-K",
    "-M",
    "-t",
    "-f",
    "-l",
    "-L",
    "-s",
    "-A",
    "--parallel",
};

bool ArgIsFlag(const std::string &arg) {
    return !arg.empty() && arg[0] == '-';
}

} // namespace

bool FastReconcile::NeedsPassthrough(const std::vector<std::string> &args) {
    for (size_t i = 0; i < args.size(); ++i) {
        const std::string &arg = args[i];
        if (kPassthroughFlags.count(arg) > 0) {
            return true;
        }
        if (arg.rfind("--parallel", 0) == 0) {
            return true;
        }
    }
    return false;
}

void FastReconcile::run(const std::vector<std::string> &args) {
    g_options.set_command(name);

    if (NeedsPassthrough(args)) {
        std::vector<std::string> recArgs = args;
        m_commands->run_p4_passthrough("reconcile", recArgs);
        return;
    }

    P5 &p5 = m_commands->p5();
    p5.SetTagProtocol();

    const bool anyRestrict = m_flagEdit || m_flagAdd || m_flagDelete;
    reconcile::ReconcileOptions opts;
    opts.preview = m_preview;
    opts.edit = m_flagEdit || !anyRestrict;
    opts.add = m_flagAdd || !anyRestrict;
    opts.deleteFiles = m_flagDelete || !anyRestrict;
    opts.mtimeOptimize = m_mtime;
    opts.skipIgnore = m_skipIgnore;
    opts.changelist = m_changelist;

    const std::string workDir = fs::current_path().string();

    reconcile::WorkspaceCache cache;
    const std::string clientName = g_options.client();
    if (!clientName.empty()) {
        reconcile::LoadDigestCache(clientName, cache);
    }

    const auto analyzed = reconcile::Analyze(p5, workDir, args, opts, cache);

    if (!clientName.empty()) {
        reconcile::SaveDigestCache(clientName, cache);
    }

    reconcile::ApplyPlan(p5, analyzed.plan, analyzed.depot, opts);
}

void FastReconcile::register_cli(CLI::App &app) {
    auto *sub = app.add_subcommand(name, description);
    sub->prefix_command();
    for (const std::string &alias : aliases) {
        sub->alias(alias);
    }

    sub->add_flag("-n", m_preview, "Preview reconcile without opening files");
    sub->add_option("-c", m_changelist, "Open files in the specified pending changelist");
    sub->add_flag("-e", m_flagEdit, "Reconcile files modified outside of P4 (open for edit)");
    sub->add_flag("-a", m_flagAdd, "Reconcile new files (open for add)");
    sub->add_flag("-d", m_flagDelete, "Reconcile deleted files (open for delete)");
    sub->add_flag("-m", m_mtime, "Skip digest when file mtime matches sync time");
    sub->add_flag("-I", m_skipIgnore, "Do not apply P4IGNORE rules");

    sub->callback([this, sub]() {
        try {
            this->run(sub->remaining());
        } catch (const CLI::RuntimeError &) {
            throw;
        }
    });
}
