#pragma once

#include <p4/mapapi.h>

#include <cstddef>
#include <string>
#include <vector>

/// Wrapper around P4API MapApi for client view mapping lines.
class ViewMap {
public:
    ViewMap() = default;

    static ViewMap fromLines(const std::vector<std::string> &lines);

    void insert(const std::string &line);
    void insert(const std::vector<std::string> &lines);
    void insert(const std::string &left, const std::string &right);

    void append(const std::string &line);
    void insertAt(std::size_t index, const std::string &line);
    void replaceAt(std::size_t index, const std::string &line);
    void eraseAt(std::size_t index);

    std::size_t size() const;
    std::string lineAt(std::size_t index) const;

    bool isEmpty() const;
    bool includes(const std::string &path) const;

    ViewMap reverse() const;
    std::vector<std::string> lines() const;
    std::vector<std::string> asArray() const { return lines(); }
    std::string toString() const;

    /// Validate mapping syntax and paths; returns false and sets err on failure.
    static bool validateLine(const std::string &line, std::string &err);

    /// Expand depot-only path to a full mapping line for the given client name.
    static bool expandQuickAdd(const std::string &depotPath, const std::string &clientName, std::string &line, std::string &err);

    /// Join CLI tokens into one mapping line (single token or quoted paths).
    static std::string joinMappingArgs(const std::vector<std::string> &args);

    MapApi &api() { return m_map; }
    const MapApi &api() const { return m_map; }

private:
    void rebuildFromLines(const std::vector<std::string> &lines);

    MapApi m_map;
};
