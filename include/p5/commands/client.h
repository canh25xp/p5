#pragma once

#include "commands/command.h"
#include "types/spec.h"
#include "types/client_spec.h"

#include <string>
#include <vector>

class P5;
class StrDict;

// p5 `client` / `workspace` command.
// Renamed to ClientComman to avoid P4API's global `::Client`
// Maybe we should put everything in p5 namespace ?
class ClientCommand : public Command {
    // CLI flags
    bool m_force{false};
    std::string m_template;
    std::string m_type;
    bool m_delete{false};
    bool m_output{false};
    bool m_input{false};
    std::string m_stream;
    bool m_switch{false};
    int m_change{0};
    std::string m_serverId;
    bool m_forceShelved{false};
    bool m_forceDeleteShelves{false};
    std::string m_clientName;

    p5::ClientSpec m_clientSpec;

public:
    ClientCommand() : Command("client", "Create or edit a client specification and its view", {"workspace"}) {}

    void OutputStat(StrDict *varList) override;

    const p5::ClientSpec &clientSpec() const { return m_clientSpec; }

    static void Load(P5 &p5, const std::vector<std::string> &args);
    static void Save(P5 &p5, const std::string &specForm, const std::vector<std::string> &args = {"-i"});
    static void Save(P5 &p5, const Spec &spec, const std::vector<std::string> &args = {"-i"});

    /// Set Client/Root/Host and return updated spec (mirror use case).
    static Spec Patch(Spec spec, const std::string &clientName, const std::string &root, const std::string &host);

    static bool IsValidName(const std::string &name);

    /// Print formatted client spec to stdout
    void Print() const;

    void run(const std::vector<std::string> &args) override;
    void register_cli(CLI::App &app) override;

private:
    void populateFromSpec(const Spec &spec);
    std::vector<std::string> buildP4Args() const;
};
