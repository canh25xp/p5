#pragma once

#include <map>
#include <optional>
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>

class StrDict;

/// P4Python Spec — validated spec field dictionary.
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
    const std::vector<std::string> &fieldOrder() const { return m_fieldOrder; }

    static Spec FromStrDict(StrDict *dict);

    /// Format as a p4 spec form suitable for `p4 <spec-cmd> -i`.
    std::string toFormText() const;

private:
    std::string canonicalKey(const std::string &key) const;

    std::map<std::string, std::string> m_fieldMap;
    std::map<std::string, Value> m_fields;
    std::vector<std::string> m_fieldOrder;
    std::string m_comment;
};

std::vector<std::string> GetViewLines(const Spec &spec);
void SetViewLines(Spec &spec, const std::vector<std::string> &lines);
std::optional<std::string> GetSpecStringField(const Spec &spec, const std::string &key);
bool HasNonEmptyStream(const Spec &spec);
