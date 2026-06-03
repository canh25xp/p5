#include <CLI/CLI.hpp>

#include "commands/mirror.h"
#include "commands/changes.h"
#include "commands.h"
#include "log.h"
#include "mirror/sync_plan.h"
#include "options.h"
#include "p5.h"
#include "types/spec.h"
#include "utils/client_resolver.h"

#include <p4/clientapi.h>

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_set>
#include <vector>

namespace fs = std::filesystem;

namespace {

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

Spec SpecFromTaggedStat(StrDict *varList) {
    Spec spec = Spec::FromStrDict(varList);
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

std::string FormatClientSpecForm(const Spec &spec) {
    static const std::unordered_set<std::string> kListFields = {
        "View",
        "SyncView",
        "UpdateView",
        "Revision",
        "Files",
        "Jobs",
        "Paths",
        "AltRoots",
    };

    std::ostringstream out;
    for (const auto &[key, value] : spec.fields()) {
        if (std::holds_alternative<std::string>(value)) {
            out << key << ":\t" << std::get<std::string>(value) << '\n';
            continue;
        }

        const auto &lines = std::get<std::vector<std::string>>(value);
        if (kListFields.count(key) > 0 || lines.size() > 1) {
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

class SpecCaptureUser : public Result {
    Spec m_spec;
    bool m_hasSpec{false};

public:
    void OutputStat(StrDict *varList) override {
        m_spec = SpecFromTaggedStat(varList);
        m_hasSpec = true;
    }

    const Spec &spec() const { return m_spec; }
    bool hasSpec() const { return m_hasSpec; }
};

class SpecInputUser : public Result {
    std::string m_spec;

public:
    explicit SpecInputUser(std::string spec) : m_spec(std::move(spec)) {}

    void InputData(StrBuf *strbuf, Error *e) override {
        strbuf->Set(m_spec.c_str());
        e->Clear();
    }
};

std::string DefaultMirrorName(const std::string &user, const std::string &templateClient) {
    std::string prefix;
    prefix.reserve(user.size());
    for (char c : user) {
        if (c == '.') {
            prefix += '_';
        } else {
            prefix += static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
        }
    }
    return prefix + "_" + templateClient;
}

bool IsValidClientName(const std::string &name) {
    if (name.empty()) {
        return false;
    }
    for (char c : name) {
        if (c == ' ' || c == '/' || c == '\\' || c == '@') {
            return false;
        }
    }
    return true;
}

std::string PatchClientSpec(Spec spec, const std::string &clientName, const std::string &root, const std::string &host) {
    spec.set("Client", clientName);
    spec.set("Root", root);
    spec.set("Host", host);
    return FormatClientSpecForm(spec);
}

std::string HaveChangesPath(const std::string &client) {
    return "//" + client + "/...#have";
}

std::vector<int> LoadHaveChangelists(P5 &p5, const std::string &client) {
    std::vector<Changes::ChangeNumber> cls = Changes::Load(p5, {"-c", client, HaveChangesPath(client)});
    std::sort(cls.begin(), cls.end());
    cls.erase(std::unique(cls.begin(), cls.end()), cls.end());
    return cls;
}

void PrintChangelistList(std::ostream &out, const std::vector<int> &cls) {
    for (size_t i = 0; i < cls.size(); ++i) {
        if (i > 0) {
            out << ' ';
        }
        out << cls[i];
    }
    out << '\n';
}

} // namespace

void Mirror::run(const std::vector<std::string> &args) {
    g_options.set_command(name);

    const std::string templateClient = args.empty() ? m_templateClient : args[0];
    if (templateClient.empty()) {
        CLI_ERROR("Template client name is required");
    }

    std::string mirrorClient = args.size() > 1 ? args[1] : m_mirrorClient;
    if (mirrorClient.empty()) {
        mirrorClient = DefaultMirrorName(g_options.user(), templateClient);
    }

    if (!IsValidClientName(mirrorClient)) {
        CLI_ERROR("Invalid mirror client name: " << mirrorClient);
    }

    P5 &p5 = m_commands->p5();

    const std::string cwd = ClientResolver::GetCurrentWorkingDirectory();
    if (cwd.empty()) {
        CLI_ERROR("Could not determine current working directory");
    }

    fs::path rootPath = fs::path(cwd) / mirrorClient;
    const std::string root = rootPath.string();
    const std::string host = ClientResolver::GetCurrentHostname();

    INFO("Creating mirror client " << mirrorClient << " from template " << templateClient);

    SpecCaptureUser capture;
    p5.SetSpecProtocol();
    p5.Run("client", {"-o", "-t", templateClient, mirrorClient}, capture);
    if (capture.IsError()) {
        CLI_ERROR("Failed to read client spec for template " << templateClient);
    }
    if (!capture.hasSpec() || capture.spec().fields().empty()) {
        CLI_ERROR("Empty client spec returned for template " << templateClient);
    }

    const std::string patched = PatchClientSpec(capture.spec(), mirrorClient, root, host);
    SpecInputUser input(patched);
    p5.Run("client", {"-i"}, input);
    if (input.IsError()) {
        CLI_ERROR("Failed to create mirror client " << mirrorClient);
    }

    std::error_code ec;
    fs::create_directories(rootPath, ec);
    if (ec) {
        CLI_ERROR("Could not create workspace root " << root << ": " << ec.message());
    }

    ClientResolver::WriteP4Config(root, mirrorClient);

    std::vector<int> templateCls = LoadHaveChangelists(p5, templateClient);
    if (templateCls.empty()) {
        WARN("Template client " << templateClient << " has no synced changelists (#have); skipping sync");
        std::cout << "Mirror client " << mirrorClient << " created at " << root << " (no sync history to replicate)\n";
        return;
    }

    const std::vector<mirror::SyncStep> plan = mirror::BuildSyncPlan(templateCls);
    INFO("Syncing " << templateCls.size() << " changelists in " << plan.size() << " step(s)");

    for (const mirror::SyncStep &step : plan) {
        const std::string revision = mirror::FormatSyncRevision(step);
        INFO("Running: p4 sync -c " << mirrorClient << " ..." << revision);
        Result syncResult = p5.Run("sync", {"-c", mirrorClient, "..." + revision});
        if (syncResult.IsError()) {
            CLI_ERROR("Sync failed at changelist step " << revision);
        }
    }

    const std::vector<int> mirrorCls = LoadHaveChangelists(p5, mirrorClient);
    if (templateCls != mirrorCls) {
        std::cerr << "Verification failed: #have changelists do not match\n";
        std::cerr << "Template (" << templateClient << "): ";
        PrintChangelistList(std::cerr, templateCls);
        std::cerr << "Mirror (" << mirrorClient << "): ";
        PrintChangelistList(std::cerr, mirrorCls);
        throw CLI::RuntimeError(1);
    }

    std::cout << "Mirror client " << mirrorClient << " created at " << root << '\n';
    std::cout << "Replicated " << templateCls.size() << " changelist(s) in " << plan.size() << " sync step(s)\n";
}

void Mirror::register_cli(CLI::App &app) {
    auto *sub = app.add_subcommand(name, description);

    sub->add_option("template", m_templateClient, "Template client to mirror")->required();
    sub->add_option("mirror", m_mirrorClient, "Mirror client name (default: USER_TEMPLATE)");

    sub->callback([this]() {
        std::vector<std::string> args;
        if (!m_templateClient.empty()) {
            args.push_back(m_templateClient);
        }
        if (!m_mirrorClient.empty()) {
            args.push_back(m_mirrorClient);
        }
        this->run(args);
    });
}
