#pragma once

#include "command.h"

#include <iosfwd>
#include <vector>
#include <string>
#include <unordered_map>

class Users : public Command {
public:
    using UserID = std::string;

    struct UserData {
        std::string fullName;
        std::string email;
    };

    Users() : Command("users", "List Perforce users (p5 formatted output)") {}

private:
    std::unordered_map<UserID, UserData> m_Users;

public:
    const std::unordered_map<UserID, UserData> &GetUserEmails() const { return m_Users; }

    void OutputStat(StrDict *varList) override;

    /// One line per user, sorted by user id: UserId  FullName  Email (columns aligned)
    void PrintSortedTsv(std::ostream &out) const;

    void run(const std::vector<std::string> &args) override;
};
