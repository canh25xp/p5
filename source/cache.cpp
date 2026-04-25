#include "cache.h"
#include <fstream>
#include <filesystem>
#include <iostream>
#include <functional>
#include "log.h"
#include "p4/clientapi.h"
#include "p4/strtable.h"

namespace fs = std::filesystem;

static void WriteString(std::ofstream &out, const std::string &s) {
    uint32_t len = s.length();
    out.write(reinterpret_cast<const char *>(&len), sizeof(len));
    out.write(s.data(), len);
}

static std::string ReadString(std::ifstream &in) {
    uint32_t len = 0;
    if (!in.read(reinterpret_cast<char *>(&len), sizeof(len)))
        return "";
    std::string s(len, '\0');
    if (len > 0)
        in.read(&s[0], len);
    return s;
}

bool CacheManager::ShouldCacheCommand(const std::string &command, std::chrono::seconds &outValidPeriod) {
    if (command == "clients" || command == "workspaces") {
        outValidPeriod = std::chrono::hours(12);
        return true;
    }
    if (command == "users") {
        outValidPeriod = std::chrono::hours(24);
        return true;
    }
    if (command == "changes" || command == "changelists") {
        outValidPeriod = std::chrono::minutes(15);
        return true;
    }
    return false;
}

std::string CacheManager::GetCacheFilePath(const std::string &command, const std::vector<std::string> &args, const std::string &port, const std::string &user) {
    std::string keyStr = port + "|" + user + "|" + command;
    for (const auto &a : args) {
        keyStr += "|" + a;
    }

    std::hash<std::string> hasher;
    size_t hash = hasher(keyStr);

    fs::path cacheDir = fs::temp_directory_path() / "p5_cache";
    std::error_code ec;
    if (!fs::exists(cacheDir, ec)) {
        fs::create_directories(cacheDir, ec);
    }

    fs::path cacheFile = cacheDir / (command + "_" + std::to_string(hash) + ".bin");
    return cacheFile.string();
}

bool CacheManager::ReplayCache(const std::string &cacheFilePath, ClientUser *targetUser, std::chrono::seconds validPeriod, bool forceUpdate) {
    if (forceUpdate)
        return false;

    std::error_code ec;
    if (!fs::exists(cacheFilePath, ec))
        return false;

    auto ftime = fs::last_write_time(cacheFilePath, ec);
    if (ec)
        return false;
    auto now = fs::file_time_type::clock::now();

    if (now - ftime > validPeriod) {
        return false; // Expired
    }

    std::ifstream in(cacheFilePath, std::ios::binary);
    if (!in.is_open())
        return false;

    std::string magic = ReadString(in);
    if (magic != "P5CACHE1")
        return false;

    while (in && in.peek() != EOF) {
        uint8_t type = 0;
        if (!in.read(reinterpret_cast<char *>(&type), sizeof(type)))
            break;

        if (type == 1) { // OutputStat
            uint32_t count = 0;
            if (!in.read(reinterpret_cast<char *>(&count), sizeof(count)))
                break;
            StrBufDict dict;
            for (uint32_t i = 0; i < count; ++i) {
                std::string k = ReadString(in);
                std::string v = ReadString(in);
                dict.SetVar(k.c_str(), v.c_str());
            }
            targetUser->OutputStat(&dict);
        } else if (type == 2) { // OutputText
            std::string text = ReadString(in);
            targetUser->OutputText(text.data(), text.length());
        } else if (type == 3) { // OutputInfo
            char level = 0;
            if (!in.read(&level, 1))
                break;
            std::string text = ReadString(in);
            targetUser->OutputInfo(level, text.c_str());
        } else if (type == 4) { // OutputError
            std::string text = ReadString(in);
            targetUser->OutputError(text.c_str());
        } else if (type == 5) { // Message
            std::string text = ReadString(in);
            Error e;
            e.Set(E_FAILED, text.c_str());
            targetUser->Message(&e);
        } else if (type == 6) { // HandleError
            std::string text = ReadString(in);
            Error e;
            e.Set(E_FAILED, text.c_str());
            targetUser->HandleError(&e);
        } else {
            break; // unknown type
        }
    }
    return true;
}

void CacheManager::ClearCache(const std::string &cacheFilePath) {
    std::error_code ec;
    if (fs::exists(cacheFilePath, ec)) {
        fs::remove(cacheFilePath, ec);
    }
}

// CachingClientUser Implementation
CachingClientUser::CachingClientUser(ClientUser *target, const std::string &cacheFilePath)
    : m_Target(target), m_CacheFilePath(cacheFilePath), m_CacheFile(nullptr) {
    std::ofstream *out = new std::ofstream(cacheFilePath, std::ios::binary);
    if (out->is_open()) {
        WriteString(*out, "P5CACHE1");
        m_CacheFile = out;
    } else {
        delete out;
        m_CacheFile = nullptr;
    }
}

CachingClientUser::~CachingClientUser() {
    if (m_CacheFile) {
        std::ofstream *out = static_cast<std::ofstream *>(m_CacheFile);
        out->close();
        delete out;
    }
}

void CachingClientUser::OutputStat(StrDict *varList) {
    if (m_CacheFile) {
        std::ofstream *out = static_cast<std::ofstream *>(m_CacheFile);
        uint8_t type = 1;
        out->write(reinterpret_cast<const char *>(&type), sizeof(type));

        uint32_t count = 0;
        StrRef var, val;
        for (int i = 0; varList->GetVar(i, var, val); ++i)
            count++;

        out->write(reinterpret_cast<const char *>(&count), sizeof(count));

        for (int i = 0; varList->GetVar(i, var, val); ++i) {
            WriteString(*out, var.Text());
            WriteString(*out, val.Text());
        }
    }
    if (m_Target)
        m_Target->OutputStat(varList);
}

void CachingClientUser::OutputText(const char *data, int length) {
    if (m_CacheFile) {
        std::ofstream *out = static_cast<std::ofstream *>(m_CacheFile);
        uint8_t type = 2;
        out->write(reinterpret_cast<const char *>(&type), sizeof(type));
        WriteString(*out, std::string(data, length));
    }
    if (m_Target)
        m_Target->OutputText(data, length);
}

void CachingClientUser::OutputInfo(char level, const char *data) {
    if (m_CacheFile) {
        std::ofstream *out = static_cast<std::ofstream *>(m_CacheFile);
        uint8_t type = 3;
        out->write(reinterpret_cast<const char *>(&type), sizeof(type));
        out->write(&level, 1);
        WriteString(*out, std::string(data));
    }
    if (m_Target)
        m_Target->OutputInfo(level, data);
}

void CachingClientUser::OutputError(const char *errBuf) {
    if (m_CacheFile) {
        std::ofstream *out = static_cast<std::ofstream *>(m_CacheFile);
        uint8_t type = 4;
        out->write(reinterpret_cast<const char *>(&type), sizeof(type));
        WriteString(*out, std::string(errBuf));
    }
    if (m_Target)
        m_Target->OutputError(errBuf);
}

void CachingClientUser::OutputBinary(const char *data, int length) {
    if (m_Target)
        m_Target->OutputBinary(data, length);
}

void CachingClientUser::Message(Error *err) {
    if (m_CacheFile) {
        std::ofstream *out = static_cast<std::ofstream *>(m_CacheFile);
        uint8_t type = 5;
        out->write(reinterpret_cast<const char *>(&type), sizeof(type));
        StrBuf msg;
        err->Fmt(&msg);
        WriteString(*out, std::string(msg.Text()));
    }
    if (m_Target)
        m_Target->Message(err);
}

void CachingClientUser::HandleError(Error *err) {
    if (m_CacheFile) {
        std::ofstream *out = static_cast<std::ofstream *>(m_CacheFile);
        uint8_t type = 6;
        out->write(reinterpret_cast<const char *>(&type), sizeof(type));
        StrBuf msg;
        err->Fmt(&msg);
        WriteString(*out, std::string(msg.Text()));
    }
    if (m_Target)
        m_Target->HandleError(err);
}
