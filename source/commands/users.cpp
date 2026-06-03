#include <CLI/CLI.hpp>

#include "commands.h"
#include "p5.h"
#include "options.h"

#include <p4/clientapi.h>

#include "log.h"
#include "utils/tabular_renderer.h"

#include <algorithm>
#include <iostream>
#include <ostream>
#include <vector>

void Users::OutputStat(StrDict *varList) {
    StrPtr *userIDPtr = varList->GetVar("User");

    if (!userIDPtr) {
        ERROR("User not found for a Perforce user");
        return;
    }

    UserID userID = userIDPtr->Text();
    UserData userData;

    StrPtr *emailPtr = varList->GetVar("Email");
    if (emailPtr) {
        userData.email = emailPtr->Text();
    }

    StrPtr *fullNamePtr = varList->GetVar("FullName");
    if (fullNamePtr) {
        userData.fullName = fullNamePtr->Text();
    } else {
        userData.fullName = userID;
    }

    m_Users.insert({userID, userData});
}

void Users::PrintSortedTsv(std::ostream &out) const {
    if (m_Users.empty()) {
        return;
    }

    std::vector<UserID> ids;
    ids.reserve(m_Users.size());
    for (const auto &entry : m_Users) {
        ids.push_back(entry.first);
    }
    std::sort(ids.begin(), ids.end());

    std::vector<std::vector<std::string>> rows;
    rows.reserve(ids.size());
    for (const UserID &id : ids) {
        const UserData &data = m_Users.at(id);
        rows.push_back({id, data.fullName, data.email});
    }

    const std::vector<std::string> headers = {"User", "FullName", "Email"};
    p5::PrintTable(out, headers, rows);
}

void Users::run(const std::vector<std::string> &args) {
    g_options.set_command(name);
    P5 &p5 = m_commands->p5();
    Users r = p5.RunUsers(args);
    if (r.IsError())
        throw CLI::RuntimeError(1);

    r.PrintSortedTsv(std::cout);
}
