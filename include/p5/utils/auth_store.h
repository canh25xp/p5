#pragma once

#include <string>

class AuthStore {
public:
    static bool SavePassword(const std::string &port, const std::string &user, const std::string &password);
    static bool LoadPassword(const std::string &port, const std::string &user, std::string &password);
};
