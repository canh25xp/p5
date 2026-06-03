#include "types/workspace.h"

namespace p5 {

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

} // namespace p5
