#include <CLI/CLI.hpp>

#include "commands/client.h"
#include "commands.h"
#include "log.h"
#include "options.h"
#include "p5.h"
#include "types/client_spec.h"

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

void ClientCommand::OutputStat(StrDict *varList) {
    Spec m_spec = Spec::FromTaggedStat(varList);
    m_clientSpec = p5::ParseClientSpec(m_spec);
}

std::vector<std::string> ClientCommand::buildP4Args() const {
    std::vector<std::string> args;

    if (m_force) {
        args.push_back("-f");
    }
    if (!m_template.empty()) {
        args.push_back("-t");
        args.push_back(m_template);
    }
    if (!m_type.empty()) {
        args.push_back("-T");
        args.push_back(m_type);
    }
    if (m_delete) {
        args.push_back("-d");
    }
    if (m_output) {
        args.push_back("-o");
    }
    if (m_input) {
        args.push_back("-i");
    }
    if (!m_stream.empty()) {
        args.push_back("-S");
        args.push_back(m_stream);
    }
    if (m_switch) {
        args.push_back("-s");
    }
    if (m_change != 0) {
        args.push_back("-c");
        args.push_back(std::to_string(m_change));
    }
    if (!m_serverId.empty()) {
        args.push_back("--serverid=" + m_serverId);
    }
    if (m_forceShelved) {
        args.push_back("-Fs");
    }
    if (m_forceDeleteShelves) {
        args.push_back("-Fd");
    }
    if (!m_clientName.empty()) {
        args.push_back(m_clientName);
    }

    return args;
}

void ClientCommand::Load(P5 &p5, const std::vector<std::string> &args) {
    ClientCommand r = p5.RunClient(args);
    if (r.IsError()) {
        CLI_ERROR("Failed to read client spec");
    }
    r.Print();
}

void ClientCommand::Save(P5 &p5, const std::string &specForm, const std::vector<std::string> &args) {
    SpecInputUser input(specForm);
    p5.SetSpecProtocol();
    p5.Run("client", args, input);
    if (input.IsError()) {
        CLI_ERROR("Failed to save client spec");
    }
}

void ClientCommand::Save(P5 &p5, const Spec &spec, const std::vector<std::string> &args) {
    Save(p5, spec.ToForm(), args);
}

Spec ClientCommand::Patch(Spec spec, const std::string &clientName, const std::string &root, const std::string &host) {
    spec.set("Client", clientName);
    spec.set("Root", root);
    spec.set("Host", host);
    return spec;
}

bool ClientCommand::IsValidName(const std::string &name) {
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

void ClientCommand::Print() const {
    m_clientSpec.Print();
}

void ClientCommand::run(const std::vector<std::string> &args) {
    g_options.set_command(name);
    P5 &p5 = m_commands->p5();

    if (m_output) {
        Load(p5, args);
        return;
    }

    if (m_input) {
        Save(p5, ReadStdin(), args);
        return;
    }

    m_commands->run_p4_passthrough(name.c_str(), args);
}

void ClientCommand::register_cli(CLI::App &app) {
    auto *sub = app.add_subcommand(name, description);

    for (const std::string &alias : aliases) {
        sub->alias(alias);
    }

    // Flag options
    sub->add_flag("-f", m_force, "Force operation (requires admin access)");
    sub->add_flag("-d", m_delete, "Delete the specified client");
    sub->add_flag("-o", m_output, "Output the client spec to stdout");
    sub->add_flag("-i", m_input, "Read client spec from stdin");
    sub->add_flag("-s", m_switch, "Switch client view without invoking editor");
    sub->add_flag("-F", m_forceShelved, "Force with shelved changes");
    sub->add_flag("--force-delete-shelves", m_forceDeleteShelves, "Force delete client and all shelved changes");

    // Option arguments
    sub->add_option("-t", m_template, "Use template client's View and Options");
    sub->add_option("-T", m_type, "Set client type (writeable, readonly, partitioned)");
    sub->add_option("-S", m_stream, "Bind client to a stream");
    sub->add_option("-c,--change", m_change, "Changelist number for stream view inspection (use with -S -o)");
    sub->add_option("--serverid", m_serverId, "Server ID for restricted operations");

    // Positional client name
    sub->add_option("name", m_clientName, "Client name");

    sub->allow_extras();

    sub->callback([this, sub]() {
        std::vector<std::string> args = buildP4Args();
        // Append any extra args that weren't parsed
        for (const auto &extra : sub->remaining()) {
            args.push_back(extra);
        }
        this->run(args);
    });
}
