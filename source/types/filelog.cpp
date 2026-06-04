#include "types/filelog.h"

#include "utils/tag_dict.h"

#include <p4/clientapi.h>

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <stdexcept>

namespace p5 {

namespace {

int ParseIntegrationRev(const char *value) {
    if (!value || std::strcmp(value, "none") == 0) {
        return 0;
    }
    while (*value == '#') {
        ++value;
    }
    return std::atoi(value);
}

std::string FormatRevisionTime(int64_t unixTime, bool includeTime) {
    if (unixTime <= 0) {
        return {};
    }
    std::time_t t = static_cast<std::time_t>(unixTime);
    std::tm tm_buf{};
#if defined(_WIN32)
    localtime_s(&tm_buf, &t);
#else
    localtime_r(&t, &tm_buf);
#endif
    char buf[64];
    const char *fmt = includeTime ? "%Y/%m/%d %H:%M:%S" : "%Y/%m/%d";
    if (std::strftime(buf, sizeof(buf), fmt, &tm_buf) == 0) {
        return std::to_string(unixTime);
    }
    return buf;
}

void TruncateDescription(std::string &desc, size_t maxLen) {
    if (desc.size() > maxLen) {
        desc.resize(maxLen);
    }
}

std::string FormatRevisionDescription(const std::string &desc, const FilelogFormatOptions &opts) {
    if (opts.longDesc) {
        return desc;
    }
    if (opts.truncDesc) {
        std::string truncated = desc;
        TruncateDescription(truncated, 250);
        return truncated;
    }
    const size_t newline = desc.find('\n');
    if (newline == std::string::npos) {
        return desc;
    }
    return desc.substr(0, newline);
}

std::string FormatRevisionLine(const FileRevision &rev, int revWidth, int changeWidth, const FilelogFormatOptions &opts) {
    std::ostringstream out;
    const std::string formattedDesc = FormatRevisionDescription(rev.desc, opts);
    out << "... #" << std::setw(revWidth) << std::left << rev.rev << std::right << " change " << std::setw(changeWidth) << std::left << rev.change << std::right << " "
        << std::setw(9) << std::left << rev.action << std::right << " on " << FormatRevisionTime(rev.time, opts.includeTime) << "  by " << rev.user << "@" << rev.client
        << " (" << rev.type << ") '" << formattedDesc << "'";
    return out.str();
}

std::string FormatIntegrationLine(const FileIntegration &integration) {
    std::ostringstream out;
    out << "... ... " << integration.how << " " << integration.file << "#" << integration.srev << "," << integration.erev;
    return out.str();
}

} // namespace

void FileRevision::addIntegration(std::string how, std::string file, int srev, int erev) {
    integrations.push_back(FileIntegration{std::move(how), std::move(file), srev, erev});
}

FileRevision &FilelogFile::addRevision() {
    FileRevision revision;
    revision.depotFile = depotFile;
    revisions.push_back(std::move(revision));
    return revisions.back();
}

FilelogFile ParseFilelogStat(StrDict *varList) {
    if (!varList || !HasTag(varList, "depotFile")) {
        throw std::runtime_error("Not a filelog object");
    }

    FilelogFile file;
    file.depotFile = TagScalar(varList, "depotFile");

    const std::vector<std::string> revs = TagArray(varList, "rev");
    for (size_t n = 0; n < revs.size(); ++n) {
        FileRevision &revision = file.addRevision();
        revision.rev = std::atoi(revs[n].c_str());
        revision.change = std::atoi(TagScalar(varList, "change", n).c_str());
        revision.action = TagScalar(varList, "action", n);
        revision.type = TagScalar(varList, "type", n);
        revision.time = std::strtoll(TagScalar(varList, "time", n).c_str(), nullptr, 10);
        revision.user = TagScalar(varList, "user", n);
        revision.client = TagScalar(varList, "client", n);
        revision.desc = TagScalar(varList, "desc", n);

        if (HasTag(varList, "digest")) {
            const std::string digest = TagScalar(varList, "digest", n);
            if (!digest.empty()) {
                revision.digest = digest;
            }
        }
        if (HasTag(varList, "fileSize")) {
            const std::string fileSize = TagScalar(varList, "fileSize", n);
            if (!fileSize.empty()) {
                revision.fileSize = static_cast<uint64_t>(std::strtoull(fileSize.c_str(), nullptr, 10));
            }
        }

        const std::vector<std::string> howList = TagNestedArray(varList, "how", n);
        if (howList.empty()) {
            continue;
        }

        const std::vector<std::string> files = TagNestedArray(varList, "file", n);
        const std::vector<std::string> srevs = TagNestedArray(varList, "srev", n);
        const std::vector<std::string> erevs = TagNestedArray(varList, "erev", n);

        for (size_t m = 0; m < howList.size(); ++m) {
            const std::string &how = howList[m];
            const std::string integFile = m < files.size() ? files[m] : std::string{};
            const int srev = m < srevs.size() ? ParseIntegrationRev(srevs[m].c_str()) : 0;
            const int erev = m < erevs.size() ? ParseIntegrationRev(erevs[m].c_str()) : 0;
            revision.addIntegration(how, integFile, srev, erev);
        }
    }

    return file;
}

std::string FormatFilelog(const FilelogFile &file, const FilelogFormatOptions &opts) {
    if (file.revisions.empty()) {
        return file.depotFile;
    }

    int revWidth = static_cast<int>(std::to_string(file.revisions.front().rev).size());
    int changeWidth = static_cast<int>(std::to_string(file.revisions.front().change).size());

    for (const FileRevision &revision : file.revisions) {
        revWidth = std::max(revWidth, static_cast<int>(std::to_string(revision.rev).size()));
        changeWidth = std::max(changeWidth, static_cast<int>(std::to_string(revision.change).size()));
    }

    std::ostringstream out;
    out << file.depotFile;
    for (const FileRevision &revision : file.revisions) {
        out << '\n'
            << FormatRevisionLine(revision, revWidth, changeWidth, opts);
        for (const FileIntegration &integration : revision.integrations) {
            out << '\n'
                << FormatIntegrationLine(integration);
        }
    }
    return out.str();
}

} // namespace p5
