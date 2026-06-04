#pragma once

#include <p4/mapapi.h>

#include <string>
#include <vector>

/// P4Python Map wrapper around P4API MapApi.
class ViewMap {
public:
    ViewMap() = default;

    void insert(const std::string &line);
    void insert(const std::vector<std::string> &lines);
    void insert(const std::string &left, const std::string &right);

    bool isEmpty() const;
    bool includes(const std::string &path) const;

    ViewMap reverse() const;
    std::vector<std::string> asArray() const;
    std::string toString() const;

    MapApi &api() { return m_map; }
    const MapApi &api() const { return m_map; }

private:
    MapApi m_map;
};
