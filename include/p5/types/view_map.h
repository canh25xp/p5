#pragma once

#include <p4/mapapi.h>

#include <memory>
#include <string>
#include <vector>

// Represents a Perforce view mapping (client/branch/label view).
// Wraps P4API MapApi to provide mapping insertion, path inclusion testing, reversal, and serialization.
// Insert entries as "left right" pairs or single lines, then query with includes() or dump with asArray()/toString().
class ViewMap {
public:
    ViewMap() : m_map(std::make_unique<MapApi>()) {}

    // Prevent copying - MapApi is not safely copyable (holds heap-allocated MapTable*)
    ViewMap(const ViewMap &) = delete;
    ViewMap &operator=(const ViewMap &) = delete;

    // Allow move semantics for return-by-value and container usage
    ViewMap(ViewMap &&) = default;
    ViewMap &operator=(ViewMap &&) = default;

    void clear() { m_map->Clear(); }

    void insert(const std::string &line);
    void insert(const std::vector<std::string> &lines);
    void insert(const std::string &left, const std::string &right);

    bool isEmpty() const;
    bool includes(const std::string &path) const;

    ViewMap reverse() const;
    std::vector<std::string> asArray() const;
    std::string toString() const;

    MapApi &api() { return *m_map; }
    const MapApi &api() const { return *m_map; }

private:
    std::unique_ptr<MapApi> m_map;
};
