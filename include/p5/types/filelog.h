#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

class StrDict;

namespace p5 {

// One integration record within a filelog revision — how the file was integrated (branch/copy/merge/edit),
// the source file path, and the start/end revision range.
struct FileIntegration {
    std::string how;
    std::string file;
    int srev{0};
    int erev{0};
};

// One revision of a depot file from p4 filelog output, including:
// change number, action, file type, timestamp, user, client, description, and any integration records.
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

// Top-level container for the filelog history of a single depot file, holding an ordered list of FileRevision entries.
struct FilelogFile {
    std::string depotFile;
    std::vector<FileRevision> revisions;

    FileRevision &addRevision();
};

FilelogFile ParseFilelogStat(StrDict *varList);

struct FilelogFormatOptions {
    bool includeTime{false};
    bool longDesc{false};
    bool truncDesc{false};
};

std::string FormatFilelog(const FilelogFile &file, const FilelogFormatOptions &opts = {});

} // namespace p5
