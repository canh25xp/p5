#pragma once

#include "commands/command.h"
#include "types/spec.h"

#include <string>
#include <vector>

class P5;
class StrDict;

// p5 `client` / `workspace` command.
// Renamed to ClientComman to avoid P4API's global `::Client`
// Maybe we should put everything in p5 namespace ?
class ClientCommand : public Command {
    Spec m_spec;
    bool m_hasSpec{false};

public:
    ClientCommand() : Command("client", "Create or edit a client specification and its view", {"workspace"}) {}

    void OutputStat(StrDict *varList) override;

    const Spec &spec() const { return m_spec; }
    bool hasSpec() const { return m_hasSpec; }

    /// Fetch client spec via `p4 client -o ...` (tag + specstring protocol).
    static Spec Load(P5 &p5, const std::vector<std::string> &args);

    /// Submit spec form via `p4 client -i ...` (default args {"-i"}; caller may add `-f`, etc.).
    static void Save(P5 &p5, const std::string &specForm, const std::vector<std::string> &args = {"-i"});
    static void Save(P5 &p5, const Spec &spec, const std::vector<std::string> &args = {"-i"});

    /// Set Client/Root/Host and return updated spec (mirror use case).
    static Spec Patch(Spec spec, const std::string &clientName, const std::string &root, const std::string &host);

    static bool IsValidName(const std::string &name);

    void run(const std::vector<std::string> &args) override;
    void register_cli(CLI::App &app) override;
};
