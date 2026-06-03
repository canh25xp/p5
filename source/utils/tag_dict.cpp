#include "utils/tag_dict.h"

#include <p4/clientapi.h>

namespace p5 {

std::vector<std::string> TagArray(StrDict *dict, const char *key) {
    std::vector<std::string> result;
    if (!dict || !key) {
        return result;
    }

    StrRef keyRef(key);
    for (int i = 0;; ++i) {
        StrPtr *value = dict->GetVar(keyRef, i);
        if (!value) {
            break;
        }
        result.push_back(value->Text());
    }

    if (result.empty()) {
        if (StrPtr *value = dict->GetVar(key)) {
            result.push_back(value->Text());
        }
    }

    return result;
}

std::vector<std::string> TagNestedArray(StrDict *dict, const char *key, size_t index) {
    std::vector<std::string> result;
    if (!dict || !key) {
        return result;
    }

    StrRef keyRef(key);
    for (int i = 0;; ++i) {
        StrPtr *value = dict->GetVar(keyRef, static_cast<int>(index), i);
        if (!value) {
            break;
        }
        result.push_back(value->Text());
    }

    return result;
}

bool HasTag(StrDict *dict, const char *key) {
    if (!dict || !key) {
        return false;
    }
    if (dict->GetVar(key)) {
        return true;
    }
    StrRef keyRef(key);
    return dict->GetVar(keyRef, 0) != nullptr;
}

std::string TagScalar(StrDict *dict, const char *key, size_t index) {
    if (!dict || !key) {
        return {};
    }
    StrRef keyRef(key);
    if (StrPtr *value = dict->GetVar(keyRef, static_cast<int>(index))) {
        return value->Text();
    }
    if (index == 0) {
        if (StrPtr *value = dict->GetVar(key)) {
            return value->Text();
        }
    }
    return {};
}

} // namespace p5
