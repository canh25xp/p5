#pragma once

#include "result.h"

#include <iosfwd>
#include <string>
#include <unordered_map>

class UsersResult : public Result {
public:
    using UserID = std::string;

    struct UserData {
        std::string fullName;
        std::string email;
    };

private:
    std::unordered_map<UserID, UserData> m_Users;

public:
    const std::unordered_map<UserID, UserData> &GetUserEmails() const { return m_Users; }

    void OutputStat(StrDict *varList) override;

    /// One line per user, sorted by user id: UserId TAB FullName TAB Email
    void PrintSortedTsv(std::ostream &out) const;
};
