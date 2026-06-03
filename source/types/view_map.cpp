#include "types/view_map.h"

#include <p4/clientapi.h>
#include <p4/error.h>

#include <cctype>
#include <sstream>
#include <stdexcept>

namespace {

std::string trim(const std::string &s) {
    std::size_t start = 0;
    while (start < s.size() && std::isspace(static_cast<unsigned char>(s[start]))) {
        ++start;
    }
    std::size_t end = s.size();
    while (end > start && std::isspace(static_cast<unsigned char>(s[end - 1]))) {
        --end;
    }
    return s.substr(start, end - start);
}

/// Split a mapping line into left and right paths (handles optional -+& prefix and quotes).
bool splitMappingPaths(const std::string &line, std::string &left, std::string &right) {
    const std::string trimmed = trim(line);
    if (trimmed.empty()) {
        return false;
    }

    std::size_t pos = 0;
    if (trimmed[pos] == '-' || trimmed[pos] == '+' || trimmed[pos] == '&') {
        ++pos;
    }
    while (pos < trimmed.size() && std::isspace(static_cast<unsigned char>(trimmed[pos]))) {
        ++pos;
    }
    if (pos + 1 >= trimmed.size() || trimmed.compare(pos, 2, "//") != 0) {
        return false;
    }

    std::size_t leftStart = pos;
    std::size_t i = pos;
    if (trimmed[i] == '"') {
        ++i;
        while (i < trimmed.size()) {
            if (trimmed[i] == '"' && (i + 1 >= trimmed.size() || trimmed[i + 1] != '"')) {
                ++i;
                break;
            }
            if (trimmed[i] == '"' && i + 1 < trimmed.size() && trimmed[i + 1] == '"') {
                i += 2;
                continue;
            }
            ++i;
        }
    } else {
        while (i < trimmed.size() && !std::isspace(static_cast<unsigned char>(trimmed[i]))) {
            ++i;
        }
    }

    left = trimmed.substr(leftStart, i - leftStart);
    while (i < trimmed.size() && std::isspace(static_cast<unsigned char>(trimmed[i]))) {
        ++i;
    }
    if (i >= trimmed.size()) {
        right.clear();
        return true;
    }

    const std::size_t rightStart = i;
    if (trimmed[i] == '"') {
        ++i;
        while (i < trimmed.size()) {
            if (trimmed[i] == '"' && (i + 1 >= trimmed.size() || trimmed[i + 1] != '"')) {
                ++i;
                break;
            }
            if (trimmed[i] == '"' && i + 1 < trimmed.size() && trimmed[i + 1] == '"') {
                i += 2;
                continue;
            }
            ++i;
        }
    } else {
        while (i < trimmed.size() && !std::isspace(static_cast<unsigned char>(trimmed[i]))) {
            ++i;
        }
    }
    right = trimmed.substr(rightStart, i - rightStart);
    return true;
}

void formatError(Error &e, std::string &err) {
    StrBuf msg;
    e.Fmt(&msg);
    err = msg.Text();
}

} // namespace

ViewMap ViewMap::fromLines(const std::vector<std::string> &lines) {
    ViewMap map;
    map.rebuildFromLines(lines);
    return map;
}

void ViewMap::rebuildFromLines(const std::vector<std::string> &lines) {
    m_map.Clear();
    for (const std::string &line : lines) {
        if (!trim(line).empty()) {
            insert(line);
        }
    }
}

void ViewMap::insert(const std::string &line) {
    StrRef ref(line.c_str());
    m_map.Insert(ref);
}

void ViewMap::insert(const std::vector<std::string> &lines) {
    for (const std::string &line : lines) {
        insert(line);
    }
}

void ViewMap::insert(const std::string &left, const std::string &right) {
    StrRef leftRef(left.c_str());
    StrRef rightRef(right.c_str());
    m_map.Insert(leftRef, rightRef);
}

void ViewMap::append(const std::string &line) {
    insert(line);
}

void ViewMap::insertAt(const std::size_t index, const std::string &line) {
    std::vector<std::string> current = lines();
    if (index > current.size()) {
        throw std::out_of_range("view map index out of range");
    }
    current.insert(current.begin() + static_cast<std::ptrdiff_t>(index), line);
    rebuildFromLines(current);
}

void ViewMap::replaceAt(const std::size_t index, const std::string &line) {
    std::vector<std::string> current = lines();
    if (index >= current.size()) {
        throw std::out_of_range("view map index out of range");
    }
    current[index] = line;
    rebuildFromLines(current);
}

void ViewMap::eraseAt(const std::size_t index) {
    std::vector<std::string> current = lines();
    if (index >= current.size()) {
        throw std::out_of_range("view map index out of range");
    }
    current.erase(current.begin() + static_cast<std::ptrdiff_t>(index));
    rebuildFromLines(current);
}

std::size_t ViewMap::size() const {
    return static_cast<std::size_t>(const_cast<MapApi &>(m_map).Count());
}

std::string ViewMap::lineAt(const std::size_t index) const {
    const std::vector<std::string> current = lines();
    if (index >= current.size()) {
        throw std::out_of_range("view map index out of range");
    }
    return current[index];
}

bool ViewMap::isEmpty() const {
    return size() == 0;
}

bool ViewMap::includes(const std::string &path) const {
    StrBuf translated;
    StrRef pathRef(path.c_str());
    return const_cast<MapApi &>(m_map).Translate(pathRef, translated) != 0;
}

ViewMap ViewMap::reverse() const {
    ViewMap reversed;
    MapApi &source = const_cast<MapApi &>(m_map);
    for (int i = 0; i < source.Count(); ++i) {
        const StrPtr *left = source.GetLeft(i);
        const StrPtr *right = source.GetRight(i);
        if (left && right) {
            reversed.insert(left->Text(), right->Text());
        } else if (left) {
            reversed.insert(left->Text());
        }
    }
    return reversed;
}

std::vector<std::string> ViewMap::lines() const {
    std::vector<std::string> result;
    MapApi &source = const_cast<MapApi &>(m_map);
    for (int i = 0; i < source.Count(); ++i) {
        const StrPtr *left = source.GetLeft(i);
        const StrPtr *right = source.GetRight(i);
        if (!left) {
            continue;
        }
        if (right && right->Length() > 0) {
            result.push_back(std::string(left->Text()) + " " + right->Text());
        } else {
            result.push_back(left->Text());
        }
    }
    return result;
}

std::string ViewMap::toString() const {
    std::ostringstream out;
    for (const std::string &line : lines()) {
        out << line << '\n';
    }
    return out.str();
}

bool ViewMap::validateLine(const std::string &line, std::string &err) {
    err.clear();
    const std::string trimmed = trim(line);
    if (trimmed.empty()) {
        err = "mapping line is empty";
        return false;
    }

    std::string left;
    std::string right;
    if (!splitMappingPaths(trimmed, left, right)) {
        err = "invalid mapping line (expected //depot/path [//client/path])";
        return false;
    }

    Error e;
    StrRef leftRef(left.c_str());
    MapApi::Validate(leftRef, &e);
    if (e.Test()) {
        formatError(e, err);
        return false;
    }

    if (!right.empty()) {
        StrRef rightRef(right.c_str());
        MapApi::Validate(rightRef, &e);
        if (e.Test()) {
            formatError(e, err);
            return false;
        }
    }

    MapApi trial;
    StrRef lineRef(trimmed.c_str());
    trial.Insert(lineRef);
    if (trial.Count() == 0) {
        err = "mapping line could not be parsed by the view map";
        return false;
    }

    return true;
}

bool ViewMap::expandQuickAdd(const std::string &depotPath, const std::string &clientName, std::string &line, std::string &err) {
    err.clear();
    const std::string path = trim(depotPath);
    if (path.size() < 3 || path.compare(0, 2, "//") != 0) {
        err = "depot path must begin with //";
        return false;
    }
    if (clientName.empty()) {
        err = "client name is required for quick-add mapping";
        return false;
    }

    const std::size_t slash = path.find('/', 2);
    if (slash == std::string::npos || slash + 1 >= path.size()) {
        err = "depot path must include a depot name and path (//depot/...)";
        return false;
    }

    const std::string depotName = path.substr(2, slash - 2);
    const std::string rest = path.substr(slash + 1);
    line = path + " //" + clientName + "/" + depotName + "/" + rest;
    return validateLine(line, err);
}

std::string ViewMap::joinMappingArgs(const std::vector<std::string> &args) {
    if (args.empty()) {
        return {};
    }
    if (args.size() == 1) {
        return args[0];
    }
    std::ostringstream out;
    for (std::size_t i = 0; i < args.size(); ++i) {
        if (i > 0) {
            out << ' ';
        }
        const std::string &arg = args[i];
        if (arg.find(' ') != std::string::npos) {
            out << '"' << arg << '"';
        } else {
            out << arg;
        }
    }
    return out.str();
}
