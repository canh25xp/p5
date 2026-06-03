#pragma once

#include <p4/clientapi.h>

#include <string>
#include <vector>

namespace p5 {

/// All values for a tagged field (parallel array). Falls back to a single scalar value.
std::vector<std::string> TagArray(StrDict *dict, const char *key);

/// Nested tag array, e.g. how[revisionIndex][integrationIndex].
std::vector<std::string> TagNestedArray(StrDict *dict, const char *key, size_t index);

bool HasTag(StrDict *dict, const char *key);

std::string TagScalar(StrDict *dict, const char *key, size_t index = 0);

} // namespace p5
