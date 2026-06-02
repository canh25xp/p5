#pragma once

#include <string>

#include "commands/clients.h"

/// Resolves the appropriate Perforce client based on the current working directory.
///
/// The resolver fetches all clients owned by the current user on the current host,
/// then matches the CWD against each client's Root and AltRoots.
/// The client whose root is the longest prefix of CWD wins.
class ClientResolver {
public:
    /// Attempt to resolve a client name for the given working directory.
    /// Returns empty string if no match is found.
    static std::string Resolve(const std::string &cwd, const Clients::ClientMap &clients);

    /// Get the current working directory using p4api's HostEnv.
    static std::string GetCurrentWorkingDirectory();

    /// Get the current hostname using p4api's HostEnv.
    static std::string GetCurrentHostname();

    /// Filter clients to only those on the given host.
    /// A client with an empty host field is considered to match any host
    /// (some clients like Swarm clients have no host set).
    static Clients::ClientMap FilterByHost(const Clients::ClientMap &clients, const std::string &host);

    /// Update P4CLIENT in the P4CONFIG file at configPath, if the file exists.
    static void WriteClientToConfig(const std::string &configPath, const std::string &resolved);
};
