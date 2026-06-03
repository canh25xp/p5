#include "types/depot.h"

#include <cstring>

namespace p5 {

namespace {

struct FileActionMapping {
    const char *name;
    FileAction action;
};

constexpr FileActionMapping kFileActions[] = {
    {"add", FileAction::Add},
    {"edit", FileAction::Edit},
    {"delete", FileAction::Delete},
    {"branch", FileAction::Branch},
    {"move/add", FileAction::MoveAdd},
    {"move/delete", FileAction::MoveDelete},
    {"integrate", FileAction::Integrate},
    {"import", FileAction::Import},
    {"purge", FileAction::Purge},
    {"archive", FileAction::Archive},
};

struct FileTypePrefix {
    const char *prefix;
    size_t len;
    FileType type;
};

// Prefix match (Perforce types may include suffixes, e.g. "text+k").
constexpr FileTypePrefix kFileTypePrefixes[] = {
    {"binary", 6, FileType::Binary},
    {"text", 4, FileType::Text},
    {"utf8", 4, FileType::Utf8},
    {"symlink", 7, FileType::Symlink},
    {"utf16", 5, FileType::Utf16},
    {"apple", 5, FileType::Apple},
    {"resource", 8, FileType::Resource},
    {"unicode", 7, FileType::Unicode},
};

} // namespace

FileAction ParseFileAction(const char *value) {
    if (!value) {
        return FileAction::Unknown;
    }
    for (const auto &entry : kFileActions) {
        if (std::strcmp(value, entry.name) == 0) {
            return entry.action;
        }
    }
    return FileAction::Unknown;
}

FileType ParseFileType(const char *value) {
    if (!value) {
        return FileType::Unknown;
    }
    for (const auto &entry : kFileTypePrefixes) {
        if (std::strncmp(value, entry.prefix, entry.len) == 0) {
            return entry.type;
        }
    }
    return FileType::Unknown;
}

DigestType DigestTypeFor(FileType type, bool &supported) {
    supported = true;
    switch (type) {
    case FileType::Binary:
        return DigestType::Binary;
    case FileType::Text:
    case FileType::Unicode:
        return DigestType::Text;
    case FileType::Utf8:
    case FileType::Utf16:
    case FileType::Symlink:
        return DigestType::Utf8;
    case FileType::Apple:
    case FileType::Resource:
        supported = false;
        return DigestType::Binary;
    default:
        supported = false;
        return DigestType::Binary;
    }
}

void DepotState::buildMapping() {
    depotMap.clear();
    clientMap.clear();
    depotMap.reserve(fileRecords.size());
    clientMap.reserve(fileRecords.size());

    for (size_t i = 0; i < fileRecords.size(); ++i) {
        auto &record = fileRecords[i];
        depotMap[record.depotFileLower] = i;

        auto it = clientMap.find(record.clientFileLower);
        if (it == clientMap.end()) {
            clientMap[record.clientFileLower] = i;
            continue;
        }

        const auto &existing = fileRecords[it->second];
        const bool existingDeleted = existing.headAction == FileAction::Delete || existing.headAction == FileAction::MoveDelete;
        const bool currentDeleted = record.headAction == FileAction::Delete || record.headAction == FileAction::MoveDelete;

        const bool shouldReplace = (!currentDeleted && existingDeleted) || (currentDeleted == existingDeleted && record.haveRev.has_value() && !existing.haveRev.has_value());
        if (shouldReplace) {
            clientMap[record.clientFileLower] = i;
        }
    }
}

DepotFileRecord *DepotState::getByClientLower(const std::string &key) {
    auto it = clientMap.find(key);
    if (it == clientMap.end()) {
        return nullptr;
    }
    return &fileRecords[it->second];
}

const DepotFileRecord *DepotState::getByClientLower(const std::string &key) const {
    auto it = clientMap.find(key);
    if (it == clientMap.end()) {
        return nullptr;
    }
    return &fileRecords.at(it->second);
}

DepotFileRecord *DepotState::getByDepotLower(const std::string &key) {
    auto it = depotMap.find(key);
    if (it == depotMap.end()) {
        return nullptr;
    }
    return &fileRecords[it->second];
}

} // namespace p5
