#include "utils/client_resolver.h"

#include "log.h"

#include <p4/hostenv.h>
#include <p4/enviro.h>
#include <p4/error.h>

#include <cstdlib>
#include <fstream>
#include <vector>
#ifdef _WIN32
#include <direct.h>
#define getcwd _getcwd
#else
#include <unistd.h>
#endif

std::string ClientResolver::Resolve(const std::string &cwd, const Clients::ClientMap &clients) {
    std::string bestClient;
    size_t bestMatchLen = 0;

    for (const auto &[name, data] : clients) {
        // Check the primary root
        if (!data.root.empty()) {
            const std::string &root = data.root;
            // CWD must start with root followed by '/' or be exactly equal to root
            if (cwd == root || (cwd.size() > root.size() && cwd.compare(0, root.size(), root) == 0 && (cwd[root.size()] == '/' || cwd[root.size()] == '\\'))) {
                if (root.size() > bestMatchLen) {
                    bestMatchLen = root.size();
                    bestClient = name;
                }
            }
        }

        // Check alternate roots
        for (const auto &altRoot : data.altRoots) {
            if (altRoot.empty()) {
                continue;
            }
            if (cwd == altRoot || (cwd.size() > altRoot.size() && cwd.compare(0, altRoot.size(), altRoot) == 0 && (cwd[altRoot.size()] == '/' || cwd[altRoot.size()] == '\\'))) {
                if (altRoot.size() > bestMatchLen) {
                    bestMatchLen = altRoot.size();
                    bestClient = name;
                }
            }
        }
    }

    return bestClient;
}

std::string ClientResolver::GetCurrentWorkingDirectory() {
    Error e;
    StrBuf cwd;
    Enviro env;
    HostEnv hostEnv;

    if (hostEnv.GetCwd(cwd, &e, &env) && !e.Test()) {
        return cwd.Text();
    }

    // Fallback to POSIX getcwd
    char *buf = getcwd(nullptr, 0);
    if (buf) {
        std::string result(buf);
        free(buf);
        return result;
    }

    return {};
}

std::string ClientResolver::GetCurrentHostname() {
    StrBuf host;
    HostEnv hostEnv;

    if (hostEnv.GetHost(host)) {
        return host.Text();
    }

    return {};
}

Clients::ClientMap ClientResolver::FilterByHost(const Clients::ClientMap &clients, const std::string &host) {
    Clients::ClientMap result;

    for (const auto &[name, data] : clients) {
        // A client with an empty host field matches any host
        // (e.g., Swarm clients often have no host set)
        if (data.host.empty() || data.host == host) {
            result.emplace(name, data);
        }
    }

    return result;
}

void ClientResolver::WriteClientToConfig(const std::string &configPath, const std::string &resolved) {
    if (configPath.empty() || configPath == "noconfig") {
        return;
    }

    std::ifstream in(configPath);
    if (!in.is_open()) {
        return;
    }

    std::vector<std::string> lines;
    std::string line;
    bool found = false;
    while (std::getline(in, line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        if (!line.empty() && line[0] != '#') {
            size_t eq = line.find('=');
            if (eq != std::string::npos && line.compare(0, eq, "P4CLIENT") == 0) {
                if (!found) {
                    lines.push_back("P4CLIENT=" + resolved);
                    found = true;
                }
                continue;
            }
        }
        lines.push_back(line);
    }
    in.close();

    if (!found) {
        lines.push_back("P4CLIENT=" + resolved);
    }

    std::ofstream out(configPath, std::ios::binary);
    if (out.is_open()) {
        for (const std::string &l : lines) {
            out << l << "\n";
        }
        INFO("Auto-resolve: updated " << configPath << " with P4CLIENT=" << resolved);
    } else {
        WARN("Auto-resolve: could not write to " << configPath);
    }
}
