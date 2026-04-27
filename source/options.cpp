#include "options.h"

Options g_options;

#include <utility>
#include <p4/clientapi.h>
#include <p4/enviro.h>
#include <p4/hostenv.h>
#include <p4/error.h>
#include "log.h"
#include <cstdlib>

#ifndef P5_VERSION_STRING
#define P5_VERSION_STRING "p5 (unknown version) (p4api unknown)"
#endif

void Options::add(CLI::App &app) {
    app.usage("[GLOBAL OPTIONS...] <subcommand> [arguments...]");

    app.set_version_flag("-V,--version", P5_VERSION_STRING)->group("Global options");

    app.add_option("-v,--verbosity", m_verbosity, "Log level: info, warn, error, fatal (default: info/error)")
        ->group("Global options")
        ->envname("P5LOGLEVEL");

    app.add_option("-u,--user", m_user, "Override P4USER (default: $P4USER)")
        ->group("Global options");
    app.add_option("-p,--port", m_port, "Override P4PORT (default: $P4PORT)")
        ->group("Global options");
    app.add_option("-c,--client", m_client, "Override P4CLIENT (default: $P4CLIENT)")
        ->group("Global options");
    app.add_flag("--resolve-client,!--no-resolve-client", m_resolve_client, "Resolve client based on CWD")
        ->group("Global options")
        ->envname("P5RESOLVECLIENT");
    app.add_option("-z", m_protocol_z, "Set Helix protocol variable (name or name=value); repeat for multiple")
        ->group("Global options");

    // Runs after argv and env merge, before any subcommand callback (CLI11 root `run_callback` order).
    app.parse_complete_callback([this]() { apply(); });
}

void Options::apply() {
    m_p4_protocol.clear();
    m_p4_protocol.reserve(m_protocol_z.size());
    for (const std::string &entry : m_protocol_z) {
        const std::string::size_type eq = entry.find('=');
        std::string key = eq == std::string::npos ? entry : entry.substr(0, eq);
        std::string value = eq == std::string::npos ? std::string() : entry.substr(eq + 1);
        if (!key.empty()) {
            m_p4_protocol.emplace_back(std::move(key), std::move(value));
        }
    }

    if (!m_env) {
        m_env = std::make_unique<Enviro>();
        load_enviro(*m_env);
    }

    if (m_verbosity.empty()) {
        if (const char *var = std::getenv("P5VERBOSITY")) {
            m_verbosity = var;
        }
    }

    if (!m_verbosity.empty()) {
        if (m_verbosity == "info")
            Log::CurrentLevel = LogLevel::Info;
        else if (m_verbosity == "warn")
            Log::CurrentLevel = LogLevel::Warn;
        else if (m_verbosity == "error")
            Log::CurrentLevel = LogLevel::Error;
        else if (m_verbosity == "fatal")
            Log::CurrentLevel = LogLevel::Fatal;
    }

    if (char *var = m_env->Get("P4USER"))
        m_user = var;
    if (char *var = m_env->Get("P4PORT"))
        m_port = var;
    if (char *var = m_env->Get("P4CLIENT"))
        m_client = var;
}

void Options::load_enviro(Enviro &env) const {
    Error e;
    StrBuf cwd;
    HostEnv host;
    host.GetCwd(cwd, &e, &env);
    env.Config(cwd);
    env.LoadEnviro(1);

    if (!m_user.empty()) {
        env.Update("P4USER", m_user.c_str());
    }
    if (!m_port.empty()) {
        env.Update("P4PORT", m_port.c_str());
    }
    if (!m_client.empty()) {
        env.Update("P4CLIENT", m_client.c_str());
    }
}
