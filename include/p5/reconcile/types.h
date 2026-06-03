#pragma once

#include <array>
#include <cstdint>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace reconcile {

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

struct HaveRecord {
    std::string depotFile;
    std::string clientFile;
    std::optional<int> haveRev;
    int64_t syncTime{0};
};

struct WorkspaceFile {
    std::string path;
    std::string pathLower;
    uint64_t size{0};
    int64_t mtimeSecs{0};
    bool filtered{false};
};

struct WorkspaceCacheEntry {
    uint64_t size{0};
    int64_t mtimeSecs{0};
    std::array<uint8_t, 16> digest{};
};

struct WorkspaceCache {
    std::unordered_map<std::string, WorkspaceCacheEntry> fileMap;
    bool outOfDate{false};
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

struct WorkspaceState {
    std::vector<WorkspaceFile> files;
    std::unordered_map<std::string, size_t> fileMap;
    size_t numFiles{0};

    void buildMapping();
    bool hasFile(const std::string &pathLower) const;
    bool hasFiltered(const std::string &pathLower) const;
    const WorkspaceFile *getFiltered(const std::string &pathLower) const;
};

struct ReconcilePlan {
    std::vector<std::string> doAdd;
    std::vector<std::string> doEdit;
    std::vector<std::string> doReopenEdit;
    std::vector<std::string> doDelete;
    std::vector<std::string> doReopenDelete;
    std::vector<std::string> doRevertAdd;
    std::vector<std::string> doRevertEdit;
    std::vector<std::string> doRevertDelete;
    std::vector<std::string> unsupportedDepotFiles;
};

struct ReconcileOptions {
    bool preview{false};
    bool edit{true};
    bool add{true};
    bool deleteFiles{true};
    bool mtimeOptimize{false};
    bool skipIgnore{false};
    int changelist{0};
};

} // namespace reconcile
