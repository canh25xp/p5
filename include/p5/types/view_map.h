#pragma once

#include <p4/mapapi.h>

#include <memory>
#include <string>
#include <vector>

// Represents a Perforce view mapping (client/branch/label view).
// Wraps P4API MapApi to provide mapping insertion, path inclusion testing, reversal, and serialization.
// Insert entries as "left right" pairs or single lines, then query with includes() or dump with asArray()/toString().
// Note: ViewMap is not copyable (MapApi holds heap-allocated MapTable*), but is moveable.
class ViewMap {
private:
    std::unique_ptr<MapApi> m_map;
    MapCase m_sensitivity;

    void insertMapping(const std::string &left, const std::string &right, const MapType mapType);
    void copyMapApiInto(MapApi &map) const;

public:
    ViewMap();
    ViewMap(const ViewMap &) = delete;
    ViewMap &operator=(const ViewMap &) = delete;
    ViewMap(ViewMap &&) noexcept;
    ViewMap &operator=(ViewMap &&) noexcept;

    bool IsInLeft(const std::string fileRevision) const;
    bool IsInRight(const std::string fileRevision) const;

    void SetCaseSensitivity(const MapCase mode);
    MapCase GetCaseSensitivity() const { return m_sensitivity; };

    // TranslateLeftToRight turn a left to a right.
    // Returns an empty string if the mapping is invalid.
    std::string TranslateLeftToRight(const std::string &path) const;

    // TranslateRightToLeft turn a right to a left.
    // Returns an empty string if the mapping is invalid.
    std::string TranslateRightToLeft(const std::string &path) const;

    // "//a/... b/..." format
    void InsertTranslationMapping(const std::vector<std::string> &mapping);

    // "//a/..." format
    void InsertPaths(const std::vector<std::string> &paths);

    // "..." format
    void InsertPrefixedPaths(const std::string prefix, const std::vector<std::string> &paths);

    void InsertFileMap(const ViewMap &src);

    // Convenience methods for backward compatibility
    void clear();
    void insert(const std::string &line);
    void insert(const std::vector<std::string> &lines);
    void insert(const std::string &left, const std::string &right);

    bool isEmpty() const;
    bool includes(const std::string &path) const;

    ViewMap reverse() const;
    std::vector<std::string> asArray() const;
    std::string toString() const;
};
