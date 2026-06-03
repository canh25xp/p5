#include "reconcile/digest.h"

#include <openssl/evp.h>

#include <algorithm>
#include <atomic>
#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <mutex>
#include <thread>
#include <vector>

namespace fs = std::filesystem;

namespace reconcile {

namespace {

std::string CachePath(const std::string &clientName) {
    const char *home = std::getenv("HOME");
    if (!home) {
        home = std::getenv("USERPROFILE");
    }
    std::string base = home ? home : ".";
    return base + "/.cache/p5/digests_" + clientName + ".bin";
}

class Md5Context {
public:
    Md5Context() {
        ctx_ = EVP_MD_CTX_new();
        if (ctx_) {
            EVP_DigestInit_ex(ctx_, EVP_md5(), nullptr);
        }
    }

    ~Md5Context() {
        if (ctx_) {
            EVP_MD_CTX_free(ctx_);
        }
    }

    Md5Context(const Md5Context &) = delete;
    Md5Context &operator=(const Md5Context &) = delete;

    bool ok() const { return ctx_ != nullptr; }

    void update(const void *data, size_t len) {
        if (ctx_) {
            EVP_DigestUpdate(ctx_, data, len);
        }
    }

    bool final(std::array<uint8_t, 16> &out) {
        if (!ctx_) {
            return false;
        }
        unsigned int len = 0;
        return EVP_DigestFinal_ex(ctx_, out.data(), &len) == 1 && len == out.size();
    }

private:
    EVP_MD_CTX *ctx_ = nullptr;
};

void UpdateTextDigest(std::istream &input, Md5Context &ctx) {
    std::string line;
    while (std::getline(input, line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        ctx.update(line.data(), line.size());
        const char nl = '\n';
        ctx.update(&nl, 1);
    }
}

} // namespace

bool DigestMatches(const WorkspaceFile &file, FileType headType, const std::array<uint8_t, 16> &expected) {
    if (ComputeDigest(file, DigestType::Binary) == expected) {
        return true;
    }
    bool supported = true;
    const DigestType dtype = DigestTypeFor(headType, supported);
    if (supported && dtype != DigestType::Binary) {
        return ComputeDigest(file, dtype) == expected;
    }
    return false;
}

bool IsUnchangedSinceSync(const WorkspaceFile &file, const std::unordered_map<std::string, HaveRecord> &haveRecords) {
    auto it = haveRecords.find(file.pathLower);
    if (it == haveRecords.end()) {
        return false;
    }
    const int64_t syncTime = it->second.syncTime;
    if (syncTime == 0) {
        return false;
    }
    const int64_t diff = file.mtimeSecs > syncTime ? file.mtimeSecs - syncTime : syncTime - file.mtimeSecs;
    return diff <= 1;
}

std::array<uint8_t, 16> ComputeDigest(const WorkspaceFile &file, DigestType type) {
    Md5Context ctx;
    if (!ctx.ok()) {
        return {};
    }

    std::ifstream in(file.path, std::ios::binary);
    if (!in) {
        return {};
    }

    if (type == DigestType::Binary) {
        std::vector<char> buffer(128 * 1024);
        while (in) {
            in.read(buffer.data(), static_cast<std::streamsize>(buffer.size()));
            const auto got = in.gcount();
            if (got > 0) {
                ctx.update(buffer.data(), static_cast<size_t>(got));
            }
        }
    } else {
        UpdateTextDigest(in, ctx);
    }

    std::array<uint8_t, 16> out{};
    if (!ctx.final(out)) {
        return {};
    }
    return out;
}

std::vector<DigestResult> ParallelComputeDigests(const std::vector<DigestWorkItem> &files, WorkspaceCache &cache) {
    std::vector<DigestResult> results(files.size());
    std::mutex cacheMutex;

    const unsigned hw = std::max(1u, std::thread::hardware_concurrency());
    std::vector<std::thread> workers;
    workers.reserve(hw);

    std::atomic<size_t> next{0};

    auto worker = [&]() {
        while (true) {
            const size_t i = next.fetch_add(1);
            if (i >= files.size()) {
                break;
            }
            const auto &item = files[i];
            const WorkspaceFile *file = item.first;
            const DigestType dtype = item.second;

            bool fromCache = false;
            {
                std::lock_guard<std::mutex> lock(cacheMutex);
                auto it = cache.fileMap.find(file->pathLower);
                if (it != cache.fileMap.end() && it->second.size == file->size && it->second.mtimeSecs == file->mtimeSecs) {
                    results[i] = {file, it->second.digest, true};
                    fromCache = true;
                }
            }
            if (fromCache) {
                continue;
            }

            const auto digest = ComputeDigest(*file, dtype);
            results[i] = {file, digest, false};

            {
                std::lock_guard<std::mutex> lock(cacheMutex);
                cache.fileMap[file->pathLower] = WorkspaceCacheEntry{file->size, file->mtimeSecs, digest};
                cache.outOfDate = true;
            }
        }
    };

    for (unsigned t = 0; t < hw; ++t) {
        workers.emplace_back(worker);
    }
    for (auto &t : workers) {
        t.join();
    }

    return results;
}

void LoadDigestCache(const std::string &clientName, WorkspaceCache &cache) {
    std::ifstream in(CachePath(clientName), std::ios::binary);
    if (!in) {
        return;
    }

    uint32_t version = 0;
    uint32_t count = 0;
    in.read(reinterpret_cast<char *>(&version), sizeof(version));
    in.read(reinterpret_cast<char *>(&count), sizeof(count));
    if (!in || version != 1) {
        return;
    }

    for (uint32_t i = 0; i < count; ++i) {
        uint32_t pathLen = 0;
        in.read(reinterpret_cast<char *>(&pathLen), sizeof(pathLen));
        if (!in || pathLen > 4096) {
            break;
        }
        std::string path(pathLen, '\0');
        in.read(path.data(), pathLen);
        WorkspaceCacheEntry entry;
        in.read(reinterpret_cast<char *>(&entry.size), sizeof(entry.size));
        in.read(reinterpret_cast<char *>(&entry.mtimeSecs), sizeof(entry.mtimeSecs));
        in.read(reinterpret_cast<char *>(entry.digest.data()), entry.digest.size());
        if (!in) {
            break;
        }
        cache.fileMap[path] = entry;
    }
    cache.outOfDate = false;
}

void SaveDigestCache(const std::string &clientName, const WorkspaceCache &cache) {
    if (!cache.outOfDate) {
        return;
    }

    const std::string path = CachePath(clientName);
    const auto slash = path.find_last_of('/');
    if (slash != std::string::npos) {
        std::error_code ec;
        fs::create_directories(path.substr(0, slash), ec);
    }

    std::ofstream out(path, std::ios::binary | std::ios::trunc);
    if (!out) {
        return;
    }

    const uint32_t version = 1;
    const uint32_t count = static_cast<uint32_t>(cache.fileMap.size());
    out.write(reinterpret_cast<const char *>(&version), sizeof(version));
    out.write(reinterpret_cast<const char *>(&count), sizeof(count));

    for (const auto &[key, entry] : cache.fileMap) {
        const uint32_t pathLen = static_cast<uint32_t>(key.size());
        out.write(reinterpret_cast<const char *>(&pathLen), sizeof(pathLen));
        out.write(key.data(), static_cast<std::streamsize>(key.size()));
        out.write(reinterpret_cast<const char *>(&entry.size), sizeof(entry.size));
        out.write(reinterpret_cast<const char *>(&entry.mtimeSecs), sizeof(entry.mtimeSecs));
        out.write(reinterpret_cast<const char *>(entry.digest.data()), entry.digest.size());
    }
}

} // namespace reconcile
