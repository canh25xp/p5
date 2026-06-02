#pragma once

#include "reconcile/types.h"

#include <array>
#include <utility>
#include <vector>

namespace reconcile {

std::array<uint8_t, 16> ComputeDigest(const WorkspaceFile &file, DigestType type);

/// Returns true if any Perforce-compatible digest of the local file matches the expected value.
bool DigestMatches(const WorkspaceFile &file, FileType headType, const std::array<uint8_t, 16> &expected);

using DigestWorkItem = std::pair<const WorkspaceFile *, DigestType>;
using DigestResult = std::tuple<const WorkspaceFile *, std::array<uint8_t, 16>, bool>;

std::vector<DigestResult> ParallelComputeDigests(const std::vector<DigestWorkItem> &files, WorkspaceCache &cache);

bool IsUnchangedSinceSync(const WorkspaceFile &file, const std::unordered_map<std::string, HaveRecord> &haveRecords);

void LoadDigestCache(const std::string &clientName, WorkspaceCache &cache);
void SaveDigestCache(const std::string &clientName, const WorkspaceCache &cache);

} // namespace reconcile
