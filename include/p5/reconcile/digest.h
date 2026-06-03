#pragma once

#include "../types.h"

#include <array>
#include <cstdint>
#include <unordered_map>
#include <utility>
#include <vector>

namespace reconcile {

std::array<uint8_t, 16> ComputeDigest(const p5::WorkspaceFile &file, p5::DigestType type);

/// Returns true if any Perforce-compatible digest of the local file matches the expected value.
bool DigestMatches(const p5::WorkspaceFile &file, p5::FileType headType, const std::array<uint8_t, 16> &expected);

using DigestWorkItem = std::pair<const p5::WorkspaceFile *, p5::DigestType>;
using DigestResult = std::tuple<const p5::WorkspaceFile *, std::array<uint8_t, 16>, bool>;

std::vector<DigestResult> ParallelComputeDigests(const std::vector<DigestWorkItem> &files, p5::WorkspaceCache &cache);

bool IsUnchangedSinceSync(const p5::WorkspaceFile &file, const std::unordered_map<std::string, p5::HaveRecord> &haveRecords);

void LoadDigestCache(const std::string &clientName, p5::WorkspaceCache &cache);
void SaveDigestCache(const std::string &clientName, const p5::WorkspaceCache &cache);

} // namespace reconcile
