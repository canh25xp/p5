#include "CLI/CLI.hpp"

#include "p5.h"

#include "p4/enviro.h"
#include "p4/error.h"
#include "p4/hostenv.h"

#include <iostream>
#include <memory>

// Candidate names covering client and related tooling; `Enviro::IsKnown` decides what the linked API supports.
// Extracted from `strings vendor/p4api/p4api-2025.1.2907437/lib/libsupp.a 2>/dev/null | rg '^P4[A-Z0-9]+$' | sort -u`
static const char *const kP4EnvVarCandidates[] = {
    "P4ALIASES",
    "P4ALTSYNC",
    "P4AUDIT",
    "P4AUTH",
    "P4BROKEROPTIONS",
    "P4CHANGE",
    "P4CHARSET",
    "P4CLIENT",
    "P4CLIENTPATH",
    "P4COLORS",
    "P4COMMANDCHARSET",
    "P4CONFIG",
    "P4DEBUG",
    "P4DESCRIPTION",
    "P4DIAGLOG",
    "P4DIFF",
    "P4DIFFUNICODE",
    "P4EDITOR",
    "P4ENVIRO",
    "P4EXTENSIONS",
    "P4FTPCHANGE",
    "P4FTPDEBUG",
    "P4FTPLOG",
    "P4FTPOPTIONS",
    "P4FTPPORT",
    "P4FTPPREFIX",
    "P4FTPSYSLOG",
    "P4FTPTEMPLATE",
    "P4HOST",
    "P4IGNORE",
    "P4INITROOT",
    "P4JOURNAL",
    "P4LANGUAGE",
    "P4LOG",
    "P4LOGINSSO",
    "P4MERGE",
    "P4MERGEUNICODE",
    "P4NAME",
    "P4PAGER",
    "P4PASSWD",
    "P4PCACHE",
    "P4PFSIZE",
    "P4POPTIONS",
    "P4PORT",
    "P4ROOT",
    "P4SSLDIR",
    "P4TARGET",
    "P4TICKETS",
    "P4TRUST",
    "P4USER",
    "P4WEBPORT",
    "P4WEBSERVICEFLAGS",
    "P4WEBVIEWER",
    "P4ZEROSYNC",
};

void load_enviro_for_set(Enviro &env) {
    Error e;
    StrBuf cwd;
    HostEnv host;
    host.GetCwd(cwd, &e, &env);
    env.Config(cwd);
    env.LoadEnviro(1);

    if (!P5::P4PORT.empty()) {
        env.Update("P4PORT", P5::P4PORT.c_str());
    }
    if (!P5::P4USER.empty()) {
        env.Update("P4USER", P5::P4USER.c_str());
    }
    if (!P5::P4CLIENT.empty()) {
        env.Update("P4CLIENT", P5::P4CLIENT.c_str());
    }
}

/// Same sources as `p4 set` with no arguments: local `Enviro` only (no `ClientApi::Init`, no server).
void print_local_enviro(bool quiet) {
    Enviro env;
    load_enviro_for_set(env);
    env.List(quiet ? 1 : 0);
}

void print_all_supported_p4_variables(bool quiet) {
    Enviro env;
    load_enviro_for_set(env);
    const int format_flags = quiet ? 1 : 0;
    for (const char *name : kP4EnvVarCandidates) {
        if (!Enviro::IsKnown(name)) {
            continue;
        }
        StrBuf formatted;
        env.Format(name, &formatted, format_flags);
        if (formatted.Length() > 0) {
            std::cout << formatted.Text() << '\n';
        } else {
            std::cout << name << "=\n";
        }
    }
}

struct SetCommandOpts {
    bool all{false};
    bool quiet{false};
};

static void run_set(CLI::App *cmd, const SetCommandOpts &opts) {
    if (opts.all) {
        if (!P5::InitializeLibraries()) {
            throw CLI::RuntimeError(1);
        }

        print_all_supported_p4_variables(opts.quiet);

        if (!P5::ShutdownLibraries()) {
            throw CLI::RuntimeError(1);
        }
        return;
    }

    const std::vector<std::string> args = cmd->remaining();

    if (args.empty()) {
        if (!P5::InitializeLibraries()) {
            throw CLI::RuntimeError(1);
        }

        print_local_enviro(opts.quiet);

        if (!P5::ShutdownLibraries()) {
            throw CLI::RuntimeError(1);
        }
    }
}

void register_set(CLI::App &app) {
    auto opts = std::make_shared<SetCommandOpts>();

    auto *sub = app.add_subcommand("set", "Set or display Perforce variables");
    sub->add_flag("-a,--all", opts->all, "List all supported P4* variables");
    sub->add_flag("-q,--quiet", opts->quiet, "When listing settings, omit where each value is stored");
    sub->callback([sub, opts]() { run_set(sub, *opts); });
}
