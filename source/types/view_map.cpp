#include "types/view_map.h"

#include <p4/clientapi.h>

#include <sstream>

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

bool ViewMap::isEmpty() const {
    return const_cast<MapApi &>(m_map).Count() == 0;
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

std::vector<std::string> ViewMap::asArray() const {
    std::vector<std::string> lines;
    MapApi &source = const_cast<MapApi &>(m_map);
    for (int i = 0; i < source.Count(); ++i) {
        const StrPtr *left = source.GetLeft(i);
        const StrPtr *right = source.GetRight(i);
        if (!left) {
            continue;
        }
        if (right && right->Length() > 0) {
            lines.push_back(std::string(left->Text()) + " " + right->Text());
        } else {
            lines.push_back(left->Text());
        }
    }
    return lines;
}

std::string ViewMap::toString() const {
    std::ostringstream out;
    for (const std::string &line : asArray()) {
        out << line << '\n';
    }
    return out.str();
}
