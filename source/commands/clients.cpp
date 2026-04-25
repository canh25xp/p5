#include "CLI/CLI.hpp"

#include "clients.h"
#include "client_resolver.h"
#include "p5.h"

#include "p4/clientapi.h"
#include "p4/hostenv.h"

#include "log.h"

#include <algorithm>
#include <iostream>
#include <ostream>
#include <vector>

void Clients::OutputStat(StrDict *varList) {
    StrPtr *clientPtr = varList->GetVar("client");

    if (!clientPtr) {
        ERROR("Client name not found in output");
        return;
    }

    ClientName clientName = clientPtr->Text();
    ClientData clientData;

    StrPtr *rootPtr = varList->GetVar("Root");
    if (rootPtr) {
        clientData.root = rootPtr->Text();
    }

    StrPtr *hostPtr = varList->GetVar("Host");
    if (hostPtr) {
        clientData.host = hostPtr->Text();
    }

    StrPtr *descPtr = varList->GetVar("Description");
    if (descPtr) {
        clientData.description = descPtr->Text();
    }

    // Collect AltRoots (AltRoots0, AltRoots1, ...)
    for (int i = 0;; ++i) {
        std::string key = "AltRoots" + std::to_string(i);
        StrPtr *altPtr = varList->GetVar(key.c_str());
        if (!altPtr) {
            break;
        }
        clientData.altRoots.push_back(altPtr->Text());
    }

    m_Clients.insert({clientName, clientData});
}

void Clients::PrintFormatted(std::ostream &out) const {
    std::vector<ClientName> names;
    names.reserve(m_Clients.size());
    for (const auto &entry : m_Clients) {
        names.push_back(entry.first);
    }
    std::sort(names.begin(), names.end());

    for (const ClientName &name : names) {
        const ClientData &data = m_Clients.at(name);
        out << name << '\n';
        out << "  root " << data.root << '\n';
        if (!data.host.empty()) {
            out << "  host " << data.host << '\n';
        }
        if (!data.description.empty()) {
            out << "  " << data.description << '\n';
        }
        for (const auto &alt : data.altRoots) {
            out << "  altRoot " << alt << '\n';
        }
        out << '\n';
    }
}

void Clients::run(const std::vector<std::string> &args) {
    bool printDone = false;
    {
        P5 p5;
        Clients r = p5.ListClients(args);
        if (r.IsError()) {
            ClientMap toPrint = r.GetClients();

            // When --here is set, filter results to only clients on the current host
            if (m_here) {
                std::string hostname = ClientResolver::GetCurrentHostname();
                toPrint = ClientResolver::FilterByHost(toPrint, hostname);
            }

            // Sort and print
            std::vector<ClientName> names;
            names.reserve(toPrint.size());
            for (const auto &entry : toPrint) {
                names.push_back(entry.first);
            }
            std::sort(names.begin(), names.end());

            for (const ClientName &name : names) {
                const ClientData &data = toPrint.at(name);
                PRINT(name);
                PRINT("  root " << data.root);
                if (!data.host.empty()) {
                    PRINT("  host " << data.host);
                }
                if (!data.description.empty()) {
                    PRINT("  " << data.description);
                }
                for (const auto &alt : data.altRoots) {
                    PRINT("  altRoot " << alt);
                }
                PRINT("");
            }
            printDone = true;
        }
    }

    if (!printDone) {
        throw CLI::RuntimeError(1);
    }
}

void Clients::register_cli(CLI::App &app) {
    auto *sub = app.add_subcommand(name, description);

    // Register aliases (e.g., "workspaces")
    for (const std::string &alias : aliases) {
        sub->alias(alias);
    }

    sub->add_flag("--me", m_me, "List clients owned by the current user");
    sub->add_flag("--here", m_here, "List clients on the current host (implies --me)");
    sub->allow_extras();
    sub->callback([this, sub]() {
        std::vector<std::string> extraArgs = sub->remaining();
        if (m_here) {
            m_me = true;
        }
        if (m_me) {
            extraArgs.push_back("--me");
        }
        this->run(extraArgs);
    });
}
