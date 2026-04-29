#include <CLI/CLI.hpp>

#include "commands/clients.h"
#include "commands.h"
#include "utils/client_resolver.h"
#include "p5.h"

#include <p4/clientapi.h>
#include <p4/hostenv.h>

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
    P5 &p5 = m_commands->p5();
    Clients r = p5.ListClients(args);
    if (r.IsError())
        throw CLI::RuntimeError(1);

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
}

void Clients::register_cli(CLI::App &app) {
    auto *sub = app.add_subcommand(name, description);

    // Register aliases (e.g., "workspaces")
    for (const std::string &alias : aliases) {
        sub->alias(alias);
    }

    sub->add_flag("--me", m_me, "List clients owned by the current user");
    sub->add_flag("--here", m_here, "List clients on the current host (implies --me)");
    sub->add_flag("-t", m_time, "Display the time as well as the date");
    sub->add_option("-u", m_user, "List clients owned by the specified user (supports wildcards)");
    sub->add_flag("--user-case-insensitive", m_userCaseInsensitive, "Treat the -u user value as a case-insensitive search pattern");
    sub->add_option("-e", m_name, "List workspaces with names matching the pattern (case-sensitive)");
    sub->add_option("-E", m_Name, "List workspaces with names matching the pattern (case-insensitive)");
    sub->add_option("-m", m_limits, "Limit output to the specified number of workspaces");
    sub->add_option("-S", m_stream, "Limit output to workspaces dedicated to the stream");
    sub->add_flag("-U", m_unloaded, "List unloaded clients");
    sub->add_flag("-a", m_all, "Display all clients, not just those bound to this server");
    sub->add_option("-s", m_server, "Display only clients bound to the specified server ID");
    sub->callback([this, sub]() {
        std::vector<std::string> args;
        if (m_here) {
            m_me = true;
        }
        if (m_me) {
            args.push_back("--me");
        }
        if (m_time) {
            args.push_back("-t");
        }
        if (!m_user.empty()) {
            args.push_back("-u");
            args.push_back(m_user);
        }
        if (m_userCaseInsensitive) {
            args.push_back("--user-case-insensitive");
        }
        if (!m_name.empty()) {
            args.push_back("-e");
            args.push_back(m_name);
        }
        if (!m_Name.empty()) {
            args.push_back("-E");
            args.push_back(m_Name);
        }
        if (m_limits > 0) {
            args.push_back("-m");
            args.push_back(std::to_string(m_limits));
        }
        if (!m_stream.empty()) {
            args.push_back("-S");
            args.push_back(m_stream);
        }
        if (m_unloaded) {
            args.push_back("-U");
        }
        if (m_all) {
            args.push_back("-a");
        }
        if (!m_server.empty()) {
            args.push_back("-s");
            args.push_back(m_server);
        }
        this->run(args);
    });
}
