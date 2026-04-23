#include "CLI/CLI.hpp"

#include "set.h"

#include "options.h"
#include "log.h"

#include "p4/clientapi.h"
#include "p4/enviro.h"
#include "p4/error.h"
#include "p4/hostenv.h"

#include <cstdlib>
#include <cstdio>
#include <cerrno>
#include <cstring>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#if !defined(_WIN32)
#include <fstream>
#include <sys/stat.h>
#endif

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

void load_enviro_for_set(Enviro &env, const Options &options) {
    Error e;
    StrBuf cwd;
    HostEnv host;
    host.GetCwd(cwd, &e, &env);
    env.Config(cwd);
    env.LoadEnviro(1);

    if (!options.port().empty()) {
        env.Update("P4PORT", options.port().c_str());
    }
    if (!options.user().empty()) {
        env.Update("P4USER", options.user().c_str());
    }
    if (!options.client().empty()) {
        env.Update("P4CLIENT", options.client().c_str());
    }
}

#if !defined(_WIN32)
static bool line_assigns_name(const std::string &line, const std::string &name) {
    if (line.empty() || line[0] == '#') {
        return false;
    }
    const size_t eq = line.find('=');
    if (eq == std::string::npos) {
        return false;
    }
    return line.compare(0, eq, name) == 0;
}

// Perforce: persist to $P4ENVIRO if set, else $HOME/.p4enviro (Helix P4ENVIRO doc).
// Prefer getenv over only Enviro::GetHome() so the CLI matches the shell (HOME=... p5 set ...).
static std::string p4enviro_path_for_write(Enviro &env) {
    if (const char *p4e = std::getenv("P4ENVIRO")) {
        if (p4e[0] != '\0') {
            return std::string(p4e);
        }
    }
    if (const char *home_env = std::getenv("HOME")) {
        if (home_env[0] != '\0') {
            std::string h(home_env);
            while (!h.empty() && (h.back() == '/' || h.back() == '\\')) {
                h.pop_back();
            }
            if (!h.empty()) {
                return h + "/.p4enviro";
            }
        }
    }
    // Fallback (e.g. no HOME: service account)
    StrBuf home;
    if (env.GetHome(home) == 0 && home.Length() > 0) {
        std::string h(home.Text());
        while (!h.empty() && (h.back() == '/' || h.back() == '\\')) {
            h.pop_back();
        }
        if (!h.empty()) {
            return h + "/.p4enviro";
        }
    }
    return std::string();
}

static int mkdirs_posix(const std::string &dir) {
    if (dir.empty() || dir == "/") {
        return 0;
    }
    if (mkdir(dir.c_str(), 0700) == 0) {
        return 0;
    }
    if (errno == EEXIST) {
        return 0;
    }
    if (errno == ENOENT) {
        const size_t p = dir.find_last_of('/');
        if (p != std::string::npos && p > 0) {
            if (mkdirs_posix(dir.substr(0, p)) != 0) {
                return -1;
            }
        }
        if (mkdir(dir.c_str(), 0700) == 0) {
            return 0;
        }
        return errno == EEXIST ? 0 : -1;
    }
    return -1;
}

static bool p4_set_persistent_unix(Enviro &env, const std::string &name, const std::string &value) {
    const std::string path = p4enviro_path_for_write(env);
    if (path.empty()) {
        ERROR("p5 set: could not determine P4ENVIRO file path (HOME or P4ENVIRO not set?)");
        return false;
    }

    struct stat st;
    const int st_r = stat(path.c_str(), &st);
    if (st_r != 0) {
        if (errno != ENOENT) {
            const std::string err = "p5 set: " + path + ": " + std::strerror(errno);
            ERROR(err);
            return false;
        }
    }
    const bool file_exists = (st_r == 0);

    std::vector<std::string> lines;
    if (file_exists) {
        std::ifstream in(path.c_str());
        if (!in) {
            const std::string err = "p5 set: could not read " + path + ": " + std::strerror(errno);
            ERROR(err);
            return false;
        }
        std::string line;
        while (std::getline(in, line)) {
            if (!line.empty() && line.back() == '\r') {
                line.pop_back();
            }
            lines.push_back(std::move(line));
        }
    }

    std::vector<std::string> out;
    out.reserve(lines.size() + 1);
    for (const std::string &line : lines) {
        if (!line_assigns_name(line, name)) {
            out.push_back(line);
        }
    }
    if (!value.empty()) {
        out.push_back(name + "=" + value);
    }

    if (out.empty()) {
        if (file_exists) {
            if (std::remove(path.c_str()) != 0) {
                const std::string err = "p5 set: could not remove " + path + ": " + std::strerror(errno);
                ERROR(err);
                return false;
            }
        }
        return true;
    }

    const size_t last_slash = path.find_last_of('/');
    if (last_slash != std::string::npos && last_slash > 0) {
        const std::string parent = path.substr(0, last_slash);
        if (mkdirs_posix(parent) != 0) {
            const std::string err = "p5 set: could not create directory " + parent + ": " + std::strerror(errno);
            ERROR(err);
            return false;
        }
    }

    const std::string tmp = path + ".p5tmp";
    {
        std::ofstream w(tmp.c_str(), std::ios::binary);
        if (!w) {
            ERROR("p5 set: could not write temporary P4ENVIRO file");
            return false;
        }
        for (size_t i = 0; i < out.size(); ++i) {
            w << out[i] << '\n';
        }
    }
    if (std::rename(tmp.c_str(), path.c_str()) != 0) {
        const int errn = errno;
        (void)std::remove(tmp.c_str());
        const std::string err = "p5 set: could not move " + tmp + " to " + path + ": " + std::strerror(errn);
        ERROR(err);
        return false;
    }
    (void)chmod(path.c_str(), 0600);
    return true;
}
#endif // !defined(_WIN32)

/// `all == false`: same as `p4 set` with no arguments (local `Enviro` only).
/// `all == true`: all supported P4* names known to the linked API.
static void print_env(bool quiet, bool all, const Options &options) {
    Enviro env;
    load_enviro_for_set(env, options);
    if (!all) {
        env.List(quiet ? 1 : 0);
        return;
    }
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

void Set::run(const std::vector<std::string> &args) {
    if (args.empty()) {
        print_env(m_quiet, m_all, m_options);
        return;
    }

    if (m_all) {
        std::cerr << "p5 set: -a/--all cannot be used with variable arguments\n";
        throw CLI::RuntimeError(1);
    }

    for (const std::string &arg : args) {
        const std::string::size_type eq = arg.find('=');
        if (eq == std::string::npos) {
            if (!arg.empty() && arg[0] == '-') {
                std::cerr << "p5 set: unknown option: " << arg << '\n';
                throw CLI::RuntimeError(1);
            }
        } else if (eq > 0 && arg[0] == '-') {
            std::cerr << "p5 set: invalid variable name: " << arg.substr(0, eq) << '\n';
            throw CLI::RuntimeError(1);
        }
    }

    Enviro env;
    load_enviro_for_set(env, m_options);
    Error e;
    const int print_quiet = m_quiet ? 1 : 0;

    for (const std::string &arg : args) {
        const std::string::size_type eq = arg.find('=');
        if (eq == std::string::npos) {
            env.Print(arg.c_str(), print_quiet);
            continue;
        }
        if (eq == 0) {
            ERROR("p5 set: empty variable name");
            throw CLI::RuntimeError(1);
        }
        const std::string name = arg.substr(0, eq);
        const std::string value = arg.substr(eq + 1);
#if defined(_WIN32)
        ERROR("p5 set: updating Perforce variables (NAME=value) is currently not supported on Windows");
        throw CLI::RuntimeError(1);
#else
        if (!p4_set_persistent_unix(env, name, value)) {
            throw CLI::RuntimeError(1);
        }
#endif
    }
}

void Set::register_cli(CLI::App &app) {
    auto *sub = app.add_subcommand(name, description);
    sub->allow_extras();
    sub->add_flag("-a,--all", m_all, "List all supported P4* variables");
    sub->add_flag("-q,--quiet", m_quiet, "When listing settings, omit where each value is stored");
    sub->callback([this, sub]() { this->run(sub->remaining()); });
}
