#pragma once

#include "command.h"

#include <iosfwd>
#include <vector>
#include <string>
#include <unordered_map>

class Clients : public Command {
public:
    using ClientName = std::string;

    struct ClientData {
        std::string root;
        std::string host;
        std::string description;
        std::vector<std::string> altRoots;
    };

    using ClientMap = std::unordered_map<ClientName, ClientData>;

    Clients() : Command("clients", "Display list of known clients (p5 formatted output)", {"workspaces"}) {}

private:
    ClientMap m_Clients;

public:
    const ClientMap &GetClients() const { return m_Clients; }

    void OutputStat(StrDict *varList) override;

    /// One block per client, sorted by client name.
    void PrintFormatted(std::ostream &out) const;

    void run(const std::vector<std::string> &args) override;
    void register_cli(CLI::App &app) override;

private:
    bool m_me{false};
    bool m_here{false};
    bool m_t{false};
    std::string m_u;
    bool m_userCaseInsensitive{false};
    std::string m_e;
    std::string m_E;
    int m_m{0};
    std::string m_S;
    bool m_U{false};
    bool m_a{false};
    std::string m_s;
};
