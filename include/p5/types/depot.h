#pragma once

#include <array>
#include <cstdint>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace p5 {

enum class FileAction {
    Add,
    Edit,
    Delete,
    Branch,
    MoveAdd,
    MoveDelete,
    Integrate,
    Import,
    Purge,
    Archive,
    Unknown,
};

enum class DigestType { Binary,
                        Text,
                        Utf8 };

enum class FileType {
    Binary,
    Text,
    Unicode,
    Utf8,
    Utf16,
    Apple,
    Resource,
    Symlink,
    Unknown,
};

FileAction ParseFileAction(const char *value);
FileType ParseFileType(const char *value);
DigestType DigestTypeFor(FileType type, bool &supported);

struct DepotFileRecord {
    std::string depotFile;
    std::string depotFileLower;
    std::string clientFile;
    std::string clientFileLower;
    std::optional<FileType> headType;
    std::optional<FileAction> headAction;
    std::optional<int> headRev;
    std::optional<int> haveRev;
    std::optional<FileAction> action;
    std::optional<uint64_t> fileSize;
    std::optional<std::array<uint8_t, 16>> digest;
};

struct DepotState {
    std::vector<DepotFileRecord> fileRecords;
    std::unordered_map<std::string, size_t> depotMap;
    std::unordered_map<std::string, size_t> clientMap;

    void buildMapping();
    DepotFileRecord *getByClientLower(const std::string &key);
    const DepotFileRecord *getByClientLower(const std::string &key) const;
    DepotFileRecord *getByDepotLower(const std::string &key);
};

} // namespace p5
