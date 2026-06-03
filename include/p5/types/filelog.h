#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

class StrDict;

namespace p5 {

/// Integration record from `p4 filelog` (P4Python Integration).
struct FileIntegration {
    std::string how;
    std::string file;
    int srev{0};
    int erev{0};
};

/// One revision in a filelog result (P4Python Revision).
struct FileRevision {
    std::string depotFile;
    int rev{0};
    int change{0};
    std::string action;
    std::string type;
    int64_t time{0};
    std::string user;
    std::string client;
    std::string desc;
    std::optional<std::string> digest;
    std::optional<uint64_t> fileSize;
    std::vector<FileIntegration> integrations;

    void addIntegration(std::string how, std::string file, int srev, int erev);
};

/// Filelog history for one depot file (P4Python DepotFile; not fstat DepotFileRecord).
struct FilelogFile {
    std::string depotFile;
    std::vector<FileRevision> revisions;

    FileRevision &addRevision();
};

FilelogFile ParseFilelogStat(StrDict *varList);

std::string FormatFilelog(const FilelogFile &file);

} // namespace p5
