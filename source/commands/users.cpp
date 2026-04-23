#include "CLI/CLI.hpp"

#include "p5.h"

#include "p4/clientapi.h"

#include "log.h"

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
    std::vector<UserID> ids;
    ids.reserve(m_Users.size());
    for (const auto &entry : m_Users) {
        ids.push_back(entry.first);
    }
    std::sort(ids.begin(), ids.end());

    // Compute column widths
    size_t maxIdWidth = 0;
    size_t maxNameWidth = 0;
    for (const UserID &id : ids) {
        const UserData &data = m_Users.at(id);
        maxIdWidth = std::max(maxIdWidth, id.size());
        maxNameWidth = std::max(maxNameWidth, data.fullName.size());
    }

    for (const UserID &id : ids) {
        const UserData &data = m_Users.at(id);
        out << id << std::string(maxIdWidth - id.size() + 2, ' ')
            << data.fullName << std::string(maxNameWidth - data.fullName.size() + 2, ' ')
            << data.email << '\n';
    }
}

void Users::run(const std::vector<std::string> &args) {
    bool printDone = false;
    {
        P5 p5;
        Users r = p5.ListUsers(args);
        // Result::IsError() returns true when there is no client-side error.
        if (r.IsError()) {
            r.PrintSortedTsv(std::cout);
            printDone = true;
        }
    }

    if (!printDone) {
        throw CLI::RuntimeError(1);
    }
}
