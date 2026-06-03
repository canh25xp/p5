#pragma once

#include "client/output_handler.h"
#include "types/spec.h"

#include <string>

class P5;

namespace p5::client_spec {

/// Fetch a client spec with tagged `p4 client -o`. Returns false on error.
bool Fetch(P5 &p5, const std::string &clientName, Spec &spec, std::string &err);

/// Save a client spec with `p4 client -i` (form text from Spec::toFormText). Returns false on error.
bool Save(P5 &p5, const Spec &spec, std::string &err);

} // namespace p5::client_spec

class ClientSpecFetchHandler : public OutputHandler {
public:
    const Spec &spec() const { return m_spec; }
    bool hasSpec() const { return m_hasSpec; }

protected:
    OutputAction onOutputStat(StrDict *varList) override;

private:
    Spec m_spec;
    bool m_hasSpec{false};
};

class ClientSpecInputHandler : public Result {
public:
    explicit ClientSpecInputHandler(std::string formText) : m_formText(std::move(formText)) {}

    void InputData(StrBuf *buf, Error *e) override;

private:
    std::string m_formText;
};
