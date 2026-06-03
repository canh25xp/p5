#pragma once

#include <cstdint>
#include <optional>
#include <string>

namespace p5 {

struct HaveRecord {
    std::string path;
    std::string depotFile;
    std::string clientFile;
    std::optional<int> haveRev;
    int64_t syncTime{0};
};

} // namespace p5
