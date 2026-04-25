#pragma once

#include "p4/clientapi.h"
#include <string>
#include <vector>
#include <chrono>

class CacheManager {
public:
    static bool ShouldCacheCommand(const std::string &command, std::chrono::seconds &outValidPeriod);

    static std::string GetCacheFilePath(const std::string &cacheDir, const std::string &command, const std::vector<std::string> &args, const std::string &port, const std::string &user);

    static bool ReplayCache(const std::string &cacheFilePath, ClientUser *targetUser, std::chrono::seconds validPeriod, bool forceUpdate);

    static void ClearCache(const std::string &cacheFilePath);
};

class CachingClientUser : public ClientUser {
    ClientUser *m_Target;
    std::string m_CacheFilePath;
    void *m_CacheFile; // std::ofstream*

public:
    CachingClientUser(ClientUser *target, const std::string &cacheFilePath);
    ~CachingClientUser() override;

    void OutputStat(StrDict *varList) override;
    void OutputText(const char *data, int length) override;
    void OutputInfo(char level, const char *data) override;
    void OutputError(const char *errBuf) override;
    void OutputBinary(const char *data, int length) override;
    void Message(Error *err) override;
    void HandleError(Error *err) override;
};
