#include "types/spec.h"

#include <p4/clientapi.h>

#include <algorithm>
#include <cctype>
#include <sstream>
#include <cstring>

namespace {

bool IsListKey(const std::string &key) {
    return key == "View" || key == "SyncView" || key == "UpdateView" || key == "Revision" || key == "Files" || key == "Jobs" || key == "Paths";
}

bool IsMetaKey(const std::string &key) {
    return key == "func" || key == "code" || key == "data" || key == "specdef" || key == "specstring" || key == "spectype";
}

std::vector<std::string> parseViewFromFormText(const std::string &form) {
    std::vector<std::string> lines;
    const std::string marker = "View:";
    const std::size_t start = form.find(marker);
    if (start == std::string::npos) {
        return lines;
    }

    std::size_t pos = start + marker.size();
    while (pos < form.size() && (form[pos] == '\n' || form[pos] == '\r' || form[pos] == ' ' || form[pos] == '\t')) {
        ++pos;
    }

    while (pos < form.size()) {
        std::size_t lineEnd = form.find('\n', pos);
        if (lineEnd == std::string::npos) {
            lineEnd = form.size();
        }

        std::string line = form.substr(pos, lineEnd - pos);
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }

        if (!line.empty() && line[0] != '\t' && line.find(':') != std::string::npos) {
            break;
        }

        if (!line.empty() && line[0] == '\t') {
            line.erase(0, 1);
            while (!line.empty() && line[0] == '\t') {
                line.erase(0, 1);
            }
            if (!line.empty()) {
                lines.push_back(line);
            }
        }

        pos = lineEnd + 1;
    }

    return lines;
}

void appendFieldOrder(std::vector<std::string> &order, const std::string &key) {
    if (std::find(order.begin(), order.end(), key) == order.end()) {
        order.push_back(key);
    }
}

void writeIndentedField(std::ostream &out, const std::string &value) {
    std::istringstream lines(value);
    std::string line;
    while (std::getline(lines, line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        out << '\t' << line << '\n';
    }
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
        if (m_fields.count(key) == 0) {
            appendFieldOrder(m_fieldOrder, key);
        }
        m_fields[key] = std::move(value);
        return;
    }

    const std::string canonical = canonicalKey(key);
    if (m_fields.count(canonical) == 0) {
        appendFieldOrder(m_fieldOrder, canonical);
    }
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
        if (key == "speccomment") {
            spec.m_comment = val.Text();
            continue;
        }
        if (key == "specFormatted") {
            spec.m_specFormatted = val.Text();
            continue;
        }
        if (IsMetaKey(key)) {
            continue;
        }
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
            appendFieldOrder(spec.m_fieldOrder, key);
            spec.m_fields[key] = list;
        } else {
            appendFieldOrder(spec.m_fieldOrder, key);
            spec.m_fields[key] = std::string(val.Text());
        }
    }

    const auto mergeListField = [&](const char *key) {
        if (spec.contains(key)) {
            return;
        }
        StrRef keyRef(key);
        if (!dict->GetVar(keyRef, 0)) {
            return;
        }
        std::vector<std::string> list;
        for (int j = 0;; ++j) {
            StrPtr *line = dict->GetVar(keyRef, j);
            if (!line) {
                break;
            }
            const std::string text = line->Text();
            if (!text.empty()) {
                list.push_back(text);
            }
        }
        if (!list.empty()) {
            spec.set(key, list);
        }
    };

    mergeListField("View");
    mergeListField("SyncView");
    mergeListField("UpdateView");
    mergeListField("ChangeView");
    mergeListField("LimitView");

    return spec;
}

std::string Spec::toFormText() const {
    std::ostringstream out;
    if (!m_comment.empty()) {
        out << m_comment;
        if (m_comment.back() != '\n') {
            out << '\n';
        }
        out << '\n';
    }

    for (const std::string &key : m_fieldOrder) {
        const auto it = m_fields.find(key);
        if (it == m_fields.end()) {
            continue;
        }

        out << key << ":\n";
        if (const auto *scalar = std::get_if<std::string>(&it->second)) {
            writeIndentedField(out, *scalar);
        } else if (const auto *list = std::get_if<std::vector<std::string>>(&it->second)) {
            for (const std::string &line : *list) {
                out << '\t' << line << '\n';
            }
        }
        out << '\n';
    }

    return out.str();
}

std::vector<std::string> GetViewLines(const Spec &spec) {
    const auto view = spec.get("View");
    if (view) {
        if (const auto *list = std::get_if<std::vector<std::string>>(&(*view))) {
            if (!list->empty()) {
                return *list;
            }
        } else if (const auto *scalar = std::get_if<std::string>(&(*view))) {
            if (!scalar->empty()) {
                return {*scalar};
            }
        }
    }

    if (!spec.specFormatted().empty()) {
        return parseViewFromFormText(spec.specFormatted());
    }

    return {};
}

void SetViewLines(Spec &spec, const std::vector<std::string> &lines) {
    spec.set("View", lines);
}

std::optional<std::string> GetSpecStringField(const Spec &spec, const std::string &key) {
    const auto value = spec.get(key);
    if (!value) {
        return std::nullopt;
    }
    if (const auto *scalar = std::get_if<std::string>(&(*value))) {
        return *scalar;
    }
    return std::nullopt;
}

bool HasNonEmptyStream(const Spec &spec) {
    const auto stream = GetSpecStringField(spec, "Stream");
    return stream.has_value() && !stream->empty();
}
