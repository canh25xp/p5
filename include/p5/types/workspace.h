#pragma once

#include <array>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace p5 {

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

struct WorkspaceState {
    std::vector<WorkspaceFile> files;
    std::unordered_map<std::string, size_t> fileMap;
    size_t numFiles{0};

    void buildMapping();
    bool hasFile(const std::string &pathLower) const;
    bool hasFiltered(const std::string &pathLower) const;
    const WorkspaceFile *getFiltered(const std::string &pathLower) const;
};

} // namespace p5
