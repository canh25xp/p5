#include "reconcile/types.h"

#include <cstring>

namespace reconcile {

namespace {

std::string ToLower(std::string s) {
    for (char &c : s) {
        if (c >= 'A' && c <= 'Z') {
            c = static_cast<char>(c - 'A' + 'a');
        }
    }
    return s;
}

} // namespace

FileAction ParseFileAction(const char *value) {
    if (!value) {
        return FileAction::Unknown;
    }
    if (std::strcmp(value, "add") == 0) {
        return FileAction::Add;
    }
    if (std::strcmp(value, "edit") == 0) {
        return FileAction::Edit;
    }
    if (std::strcmp(value, "delete") == 0) {
        return FileAction::Delete;
    }
    if (std::strcmp(value, "branch") == 0) {
        return FileAction::Branch;
    }
    if (std::strcmp(value, "move/add") == 0) {
        return FileAction::MoveAdd;
    }
    if (std::strcmp(value, "move/delete") == 0) {
        return FileAction::MoveDelete;
    }
    if (std::strcmp(value, "integrate") == 0) {
        return FileAction::Integrate;
    }
    if (std::strcmp(value, "import") == 0) {
        return FileAction::Import;
    }
    if (std::strcmp(value, "purge") == 0) {
        return FileAction::Purge;
    }
    if (std::strcmp(value, "archive") == 0) {
        return FileAction::Archive;
    }
    return FileAction::Unknown;
}

FileType ParseFileType(const char *value) {
    if (!value) {
        return FileType::Unknown;
    }
    if (std::strncmp(value, "binary", 6) == 0) {
        return FileType::Binary;
    }
    if (std::strncmp(value, "text", 4) == 0) {
        return FileType::Text;
    }
    if (std::strncmp(value, "utf8", 4) == 0) {
        return FileType::Utf8;
    }
    if (std::strncmp(value, "symlink", 7) == 0) {
        return FileType::Symlink;
    }
    if (std::strncmp(value, "utf16", 5) == 0) {
        return FileType::Utf16;
    }
    if (std::strncmp(value, "apple", 5) == 0) {
        return FileType::Apple;
    }
    if (std::strncmp(value, "resource", 8) == 0) {
        return FileType::Resource;
    }
    if (std::strncmp(value, "unicode", 7) == 0) {
        return FileType::Unicode;
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

void WorkspaceState::buildMapping() {
    fileMap.clear();
    fileMap.reserve(files.size());
    for (size_t i = 0; i < files.size(); ++i) {
        fileMap[files[i].pathLower] = i;
    }
}

bool WorkspaceState::hasFile(const std::string &pathLower) const {
    return fileMap.find(pathLower) != fileMap.end();
}

bool WorkspaceState::hasFiltered(const std::string &pathLower) const {
    auto it = fileMap.find(pathLower);
    if (it == fileMap.end()) {
        return false;
    }
    return !files[it->second].filtered;
}

const WorkspaceFile *WorkspaceState::getFiltered(const std::string &pathLower) const {
    auto it = fileMap.find(pathLower);
    if (it == fileMap.end()) {
        return nullptr;
    }
    const auto &file = files[it->second];
    if (file.filtered) {
        return nullptr;
    }
    return &file;
}

} // namespace reconcile
