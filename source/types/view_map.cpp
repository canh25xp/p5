#include "types/view_map.h"
#include "log.h"

#include <p4/clientapi.h>

#include <sstream>

const std::vector<std::string> PATH_PREFIX_DESCRIPTIONS = {
    // Order is important.
    "share ",
    "isolate ",
    "import+ ",
    "import ",
    "exclude "};
const int PATH_PREFIX_DESCRIPTION_COUNT = 5;
const int PATH_PREFIX_DESCRIPTION_EXCLUDE_INDEX_START = 4;

ViewMap::ViewMap() : m_map(std::make_unique<MapApi>()), m_sensitivity(MapCase::Sensitive) {
    // The constructor line + this m_map set are the equivalent of calling SetCaseSensitivity locally.
    m_map->SetCaseSensitivity(m_sensitivity);
}

ViewMap::ViewMap(ViewMap &&other) noexcept
    : m_map(std::move(other.m_map)), m_sensitivity(other.m_sensitivity) {
    other.m_sensitivity = MapCase::Sensitive;
}

ViewMap &ViewMap::operator=(ViewMap &&other) noexcept {
    if (this != &other) {
        m_map = std::move(other.m_map);
        m_sensitivity = other.m_sensitivity;
        other.m_sensitivity = MapCase::Sensitive;
    }
    return *this;
}

bool ViewMap::IsInLeft(const std::string fileRevision) const {
    MapApi argMap;
    argMap.SetCaseSensitivity(m_sensitivity);
    argMap.Insert(StrBuf(fileRevision.c_str()), MapType::MapInclude);

    // MapAPI is poorly written and doesn't declare things as const when it should.
    std::unique_ptr<MapApi> joinResult(MapApi::Join(const_cast<MapApi *>(m_map.get()), &argMap));
    return joinResult != nullptr;
}

bool ViewMap::IsInRight(const std::string fileRevision) const {
    StrBuf to;
    StrBuf from(fileRevision.c_str());

    // MapAPI is poorly written and doesn't declare things as const when it should.
    MapApi *ref = const_cast<MapApi *>(m_map.get());
    return ref->Translate(from, to);
}

void ViewMap::SetCaseSensitivity(const MapCase mode) {
    m_sensitivity = mode;
    m_map->SetCaseSensitivity(mode);
}

std::string ViewMap::TranslateLeftToRight(const std::string &path) const {
    StrBuf from(path.c_str());
    StrBuf to;

    // MapAPI is poorly written and doesn't declare things as const when it should.
    MapApi *ref = const_cast<MapApi *>(m_map.get());
    if (ref->Translate(from, to, MapDir::MapLeftRight)) {
        return to.Text();
    }
    return "";
}

std::string ViewMap::TranslateRightToLeft(const std::string &path) const {
    StrBuf from(path.c_str());
    StrBuf to;

    // MapAPI is poorly written and doesn't declare things as const when it should.
    MapApi *ref = const_cast<MapApi *>(m_map.get());
    if (ref->Translate(from, to, MapDir::MapRightLeft)) {
        return to.Text();
    }
    return "";
}

void ViewMap::insertMapping(const std::string &left, const std::string &right, const MapType mapType) {
    std::string mapStrLeft = left;
    mapStrLeft.erase(mapStrLeft.find_last_not_of(' ') + 1);
    mapStrLeft.erase(0, mapStrLeft.find_first_not_of(' '));

    std::string mapStrRight = right;
    mapStrRight.erase(mapStrRight.find_last_not_of(' ') + 1);
    mapStrRight.erase(0, mapStrRight.find_first_not_of(' '));

    m_map->Insert(StrBuf(mapStrLeft.c_str()), StrBuf(mapStrRight.c_str()), mapType);
}

void ViewMap::InsertTranslationMapping(const std::vector<std::string> &mapping) {
    for (size_t i = 0; i < mapping.size(); i++) {
        const std::string &view = mapping.at(i);

        size_t left = view.find('/');

        MapType mapType = getMapType(view.front());

        // TODO This also needs quote handling

        // Skip the first few characters to only match with the right half.
        size_t right = view.find("//", 3);
        if (right == std::string::npos) {
            WARN("Found a one-sided mapping, ignoring...");
            continue;
        }

        insertMapping(view.substr(left, right), view.substr(right), mapType);
    }
}

void ViewMap::InsertPaths(const std::vector<std::string> &paths) {
    for (size_t i = 0; i < paths.size(); i++) {
        const std::string &view = paths.at(i);
        insertMapping(view, view, MapType::MapInclude);
    }
}

void ViewMap::InsertFileMap(const ViewMap &src) {
    src.copyMapApiInto(*m_map);
}

void ViewMap::InsertPrefixedPaths(const std::string prefix, const std::vector<std::string> &paths) {
    for (size_t i = 0; i < paths.size(); i++) {
        MapType mapType = MapType::MapInclude;
        std::string view = paths.at(i);

        // Some paths, such as the Stream spec, can include a prefix.
        for (int j = 0; j < PATH_PREFIX_DESCRIPTION_COUNT; j++) {
            size_t match = view.find(PATH_PREFIX_DESCRIPTIONS[j]);
            if (match == 0) {
                if (j >= PATH_PREFIX_DESCRIPTION_EXCLUDE_INDEX_START) {
                    mapType = MapType::MapExclude;
                }
                view.erase(0, PATH_PREFIX_DESCRIPTIONS[j].size());
                break;
            }
        }

        view = prefix + view;
        insertMapping(view, view, mapType);
    }
}

char ViewMap::getPrefix(MapType type) {
    switch (type) {
    case MapOverlay:
        return '+';
    case MapExclude:
        return '-';
    case MapOneToMany:
        return '&';
    default:
        return ' ';
    }
}

MapType ViewMap::getMapType(char prefix) {
    switch (prefix) {
    case '+':
        return MapType::MapOverlay;
    case '-':
        return MapType::MapExclude;
    case '&':
        return MapType::MapOneToMany;
    default:
        return MapType::MapInclude;
    }
}

void ViewMap::copyMapApiInto(MapApi &map) const {
    // MapAPI is poorly written and doesn't declare things as const when it should.
    MapApi *ref = const_cast<MapApi *>(m_map.get());

    map.Clear();
    map.SetCaseSensitivity(m_sensitivity);
    for (int i = 0; i < ref->Count(); i++) {
        map.Insert(
            StrBuf(ref->GetLeft(i)->Text()),
            StrBuf(ref->GetRight(i)->Text()),
            ref->GetType(i));
    }
}

std::vector<std::string> ViewMap::AsArray() const {
    std::vector<std::string> lines;
    MapApi &source = *m_map;
    for (int i = 0; i < source.Count(); ++i) {
        const StrPtr *left = source.GetLeft(i);
        const StrPtr *right = source.GetRight(i);
        if (!left) {
            continue;
        }
        char typePrefix = getPrefix(source.GetType(i));

        if (right && right->Length() > 0) {
            lines.push_back(std::string(1, typePrefix) + left->Text() + " " + right->Text());
        } else {
            lines.push_back(std::string(1, typePrefix) + left->Text());
        }
    }
    return lines;
}

std::ostream &operator<<(std::ostream &os, const ViewMap &viewMap) {
    for (const std::string &line : viewMap.AsArray()) {
        os << line << '\n';
    }
    return os;
}
