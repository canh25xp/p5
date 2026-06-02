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
    // WARN: this call p4 clients with "tag" protocol before any other commands.
    // Result in every commands afterward force running with protocol tag (Ugly output if the commands not yet defined Output for Result)
    // TODO: Solutions:
    // - Reset protocol after done. Not possible since SetProtocol() must happen _before_ the Init() hence cannot be changed afterward.
    // - Set protocol tags by default and accept breaking compatibility with p4
    // - Set protocol by default and define output for all p4 commands. Costly but doable.
    // - Run AutoResolveClient in a separate p5 instance.
    // - Use a transient ClientApi instance within AutoResolveClient to avoid polluting the main instance.
    // - Move auto-resolution to a pre-initialization step before m_ClientAPI.Init() is called.
    // - Implement a smart Result handler that can format tagged output back to plain text for generic commands.
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
