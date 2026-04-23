#include "p4enviro_defaults.h"

#include <cstdlib>
#include <string>

#if defined(_WIN32)
#    include <stdlib.h>
#endif

void p4enviro_apply_default_if_unset() {
    if (const char *p4e = std::getenv("P4ENVIRO")) {
        if (p4e[0] != '\0') {
            return;
        }
    }
#if defined(_WIN32)
    const char *home = std::getenv("USERPROFILE");
    if (home == nullptr || home[0] == '\0') {
        return;
    }
    std::string h(home);
    while (!h.empty() && (h.back() == '/' || h.back() == '\\')) {
        h.pop_back();
    }
    if (h.empty()) {
        return;
    }
    const std::string path = h + "\\.p4enviro";
    (void)_putenv_s("P4ENVIRO", path.c_str());
#endif
}
