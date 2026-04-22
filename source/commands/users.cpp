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

    for (const UserID &id : ids) {
        const UserData &data = m_Users.at(id);
        out << id << '\t' << data.fullName << '\t' << data.email << '\n';
    }
}

void run_users(const std::vector<std::string> &args) {
    if (!P5::InitializeLibraries()) {
        throw CLI::RuntimeError(1);
    }

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

    if (!P5::ShutdownLibraries()) {
        throw CLI::RuntimeError(1);
    }

    if (!printDone) {
        throw CLI::RuntimeError(1);
    }
}
