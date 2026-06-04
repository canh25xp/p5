#pragma once

#include <map>
#include <optional>
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>

class StrDict;

// Represents a Perforce spec (client, label, branch, etc.) as a validated dictionary of named fields.
// Supports case-insensitive field lookup via an alias map, list-valued fields (View, AltRoots, etc.),
// and conversion to/from P4API StrDict and form format.
class Spec {
public:
    using Value = std::variant<std::string, std::vector<std::string>>;

    Spec() = default;
    explicit Spec(std::map<std::string, std::string> fieldMap);

    void setFieldMap(std::map<std::string, std::string> fieldMap);
    const std::map<std::string, std::string> &permittedFields() const { return m_fieldMap; }

    void set(const std::string &key, Value value);
    std::optional<Value> get(const std::string &key) const;
    bool contains(const std::string &key) const;

    const std::map<std::string, Value> &fields() const { return m_fields; }

    static Spec FromStrDict(StrDict *dict);
    static Spec FromTaggedStat(StrDict *dict);
    std::string ToForm() const;

private:
    std::string canonicalKey(const std::string &key) const;

    std::map<std::string, std::string> m_fieldMap;
    std::map<std::string, Value> m_fields;
    std::string m_comment;
};
