#include "types/spec.h"

#include <p4/clientapi.h>

#include <algorithm>
#include <cctype>

namespace {

bool IsListKey(const std::string &key) {
    return key == "View" || key == "SyncView" || key == "UpdateView" || key == "Revision" || key == "Files" || key == "Jobs" || key == "Paths";
}

} // namespace

Spec::Spec(std::map<std::string, std::string> fieldMap) : m_fieldMap(std::move(fieldMap)) {}

void Spec::setFieldMap(std::map<std::string, std::string> fieldMap) {
    m_fieldMap = std::move(fieldMap);
}

std::string Spec::canonicalKey(const std::string &key) const {
    if (m_fieldMap.empty()) {
        return key;
    }

    if (m_fields.count(key) > 0) {
        return key;
    }

    std::string lower = key;
    std::transform(lower.begin(), lower.end(), lower.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

    for (const auto &[alias, canonical] : m_fieldMap) {
        std::string aliasLower = alias;
        std::transform(aliasLower.begin(), aliasLower.end(), aliasLower.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
        if (aliasLower == lower) {
            return canonical;
        }
    }

    throw std::runtime_error("Illegal field '" + key + "'");
}

void Spec::set(const std::string &key, Value value) {
    if (!std::holds_alternative<std::string>(value) && !std::holds_alternative<std::vector<std::string>>(value)) {
        throw std::runtime_error("Illegal value type, must be string or list");
    }

    if (m_fields.count(key) > 0 || m_fieldMap.empty()) {
        m_fields[key] = std::move(value);
        return;
    }

    const std::string canonical = canonicalKey(key);
    m_fields[canonical] = std::move(value);
}

std::optional<Spec::Value> Spec::get(const std::string &key) const {
    if (m_fields.count(key) > 0) {
        return m_fields.at(key);
    }

    if (m_fieldMap.empty()) {
        return std::nullopt;
    }

    std::string lower = key;
    std::transform(lower.begin(), lower.end(), lower.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

    for (const auto &[alias, canonical] : m_fieldMap) {
        std::string aliasLower = alias;
        std::transform(aliasLower.begin(), aliasLower.end(), aliasLower.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
        if (aliasLower == lower) {
            auto it = m_fields.find(canonical);
            if (it != m_fields.end()) {
                return it->second;
            }
            return std::nullopt;
        }
    }

    return std::nullopt;
}

bool Spec::contains(const std::string &key) const {
    return get(key).has_value();
}

Spec Spec::FromStrDict(StrDict *dict) {
    Spec spec;
    if (!dict) {
        return spec;
    }

    StrRef var;
    StrRef val;
    for (int i = 0; dict->GetVar(i, var, val); ++i) {
        const std::string key = var.Text();
        if (IsListKey(key)) {
            std::vector<std::string> list;
            list.push_back(val.Text());
            StrRef keyRef(key.c_str());
            for (int j = 1;; ++j) {
                StrPtr *next = dict->GetVar(keyRef, j);
                if (!next) {
                    break;
                }
                list.push_back(next->Text());
            }
            spec.set(key, list);
        } else {
            spec.set(key, std::string(val.Text()));
        }
    }

    return spec;
}
