#include "types/spec.h"

#include <p4/clientapi.h>

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <sstream>
#include <unordered_set>

namespace {

bool IsListKey(const std::string &key) {
    return key == "View" || key == "SyncView" || key == "UpdateView" || key == "Revision" || key == "Files" || key == "Jobs" || key == "Paths" ||
           key == "AltRoots" || key == "ChangeView" || key == "LimitView";
}

bool ParseIndexedFieldKey(const std::string &key, std::string &base, int &index) {
    size_t split = key.size();
    while (split > 0 && std::isdigit(static_cast<unsigned char>(key[split - 1]))) {
        --split;
    }
    if (split == key.size() || split == 0) {
        return false;
    }
    base = key.substr(0, split);
    index = std::atoi(key.c_str() + static_cast<int>(split));
    return true;
}

const std::unordered_set<std::string> &FormListFields() {
    static const std::unordered_set<std::string> fields = {
        "View",
        "SyncView",
        "UpdateView",
        "Revision",
        "Files",
        "Jobs",
        "Paths",
        "AltRoots",
        "ChangeView",
        "LimitView",
    };
    return fields;
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

Spec Spec::FromTaggedStat(StrDict *varList) {
    Spec spec = FromStrDict(varList);
    if (!varList) {
        return spec;
    }

    std::map<std::string, std::map<int, std::string>> indexedLists;
    StrRef var;
    StrRef val;
    for (int i = 0; varList->GetVar(i, var, val); ++i) {
        const std::string key = var.Text();
        if (key == "code" || key == "status" || key == "action") {
            continue;
        }

        std::string base;
        int index = 0;
        if (!ParseIndexedFieldKey(key, base, index)) {
            continue;
        }

        indexedLists[base][index] = val.Text();
    }

    for (const auto &[base, entries] : indexedLists) {
        if (spec.contains(base)) {
            continue;
        }
        std::vector<std::pair<int, std::string>> sorted(entries.begin(), entries.end());
        std::sort(sorted.begin(), sorted.end(), [](const auto &a, const auto &b) { return a.first < b.first; });
        std::vector<std::string> lines;
        lines.reserve(sorted.size());
        for (const auto &[_, line] : sorted) {
            lines.push_back(line);
        }
        if (!lines.empty()) {
            spec.set(base, lines);
        }
    }

    return spec;
}

std::string Spec::ToForm() const {
    std::ostringstream out;
    for (const auto &[key, value] : m_fields) {
        if (std::holds_alternative<std::string>(value)) {
            out << key << ":\t" << std::get<std::string>(value) << '\n';
            continue;
        }

        const auto &lines = std::get<std::vector<std::string>>(value);
        if (FormListFields().count(key) > 0 || lines.size() > 1) {
            out << key << ":\n";
            for (const std::string &line : lines) {
                out << '\t' << line << '\n';
            }
        } else if (!lines.empty()) {
            out << key << ":\t" << lines.front() << '\n';
        }
    }
    return out.str();
}
