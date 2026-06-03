#include <CLI/CLI.hpp>

#include "commands/client.h"
#include "commands.h"
#include "log.h"
#include "options.h"
#include "p5.h"

#include <p4/clientapi.h>

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace {

class SpecInputUser : public Result {
    std::string m_spec;

public:
    explicit SpecInputUser(std::string spec) : m_spec(std::move(spec)) {}

    void InputData(StrBuf *strbuf, Error *e) override {
        strbuf->Set(m_spec.c_str());
        e->Clear();
    }
};

bool HasFlag(const std::vector<std::string> &args, const char *flag) {
    for (const std::string &arg : args) {
        if (arg == flag) {
            return true;
        }
    }
    return false;
}

std::string ReadStdin() {
    std::ostringstream input;
    input << std::cin.rdbuf();
    return input.str();
}

} // namespace

void p5::Client::OutputStat(StrDict *varList) {
    m_spec = Spec::FromTaggedStat(varList);
    m_hasSpec = true;
}

Spec p5::Client::Load(P5 &p5, const std::vector<std::string> &args) {
    p5::Client r = p5.RunClient(args);
    if (r.IsError()) {
        CLI_ERROR("Failed to read client spec");
    }
    if (!r.hasSpec() || r.spec().fields().empty()) {
        CLI_ERROR("Empty client spec returned");
    }
    return r.spec();
}

void p5::Client::Save(P5 &p5, const std::string &specForm, const std::vector<std::string> &args) {
    SpecInputUser input(specForm);
    p5.SetSpecProtocol();
    p5.Run("client", args, input);
    if (input.IsError()) {
        CLI_ERROR("Failed to save client spec");
    }
}

void p5::Client::Save(P5 &p5, const Spec &spec, const std::vector<std::string> &args) {
    Save(p5, spec.ToForm(), args);
}

Spec p5::Client::Patch(Spec spec, const std::string &clientName, const std::string &root, const std::string &host) {
    spec.set("Client", clientName);
    spec.set("Root", root);
    spec.set("Host", host);
    return spec;
}

bool p5::Client::IsValidName(const std::string &name) {
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

void p5::Client::run(const std::vector<std::string> &args) {
    g_options.set_command(name);
    P5 &p5 = m_commands->p5();

    if (HasFlag(args, "-o")) {
        const Spec spec = Load(p5, args);
        std::cout << spec.ToForm();
        return;
    }

    if (HasFlag(args, "-i")) {
        Save(p5, ReadStdin(), args);
        return;
    }

    m_commands->run_p4_passthrough(name.c_str(), args);
}

void p5::Client::register_cli(CLI::App &app) {
    auto *sub = app.add_subcommand(name, description);
    sub->prefix_command();

    for (const std::string &alias : aliases) {
        sub->alias(alias);
    }

    sub->callback([this, sub]() { this->run(sub->remaining()); });
}
