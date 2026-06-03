#include "client/client_spec.h"

#include "p5.h"

#include <p4/clientapi.h>
#include <p4/error.h>

#include <cstring>

namespace p5::client_spec {

namespace {

void formatError(const Error &e, std::string &err) {
    StrBuf msg;
    e.Fmt(&msg);
    err = msg.Text();
}

} // namespace

bool Fetch(P5 &p5, const std::string &clientName, Spec &spec, std::string &err) {
    err.clear();
    std::vector<std::string> args = {"-o"};
    if (!clientName.empty()) {
        args.push_back(clientName);
    }

    p5.SetTagProtocol();
    ClientSpecFetchHandler handler;
    p5.ExecuteCommand("client", args, handler);

    if (handler.IsError() || handler.IsFatal()) {
        formatError(handler.GetError(), err);
        if (err.empty()) {
            err = "p4 client -o failed";
        }
        return false;
    }
    if (!handler.hasSpec()) {
        err = "client spec not returned by server";
        return false;
    }

    spec = handler.spec();
    return true;
}

bool Save(P5 &p5, const Spec &spec, std::string &err) {
    err.clear();
    const std::string form = spec.toFormText();
    if (form.empty()) {
        err = "empty client spec";
        return false;
    }

    ClientSpecInputHandler handler(form);
    p5.ExecuteCommand("client", {"-i"}, handler);

    if (handler.IsError() || handler.IsFatal()) {
        formatError(handler.GetError(), err);
        if (err.empty()) {
            err = "p4 client -i failed";
        }
        return false;
    }

    return true;
}

} // namespace p5::client_spec

OutputAction ClientSpecFetchHandler::onOutputStat(StrDict *varList) {
    if (!varList) {
        return OutputAction::Report;
    }

    StrPtr *code = varList->GetVar("code");
    if (code && std::strcmp(code->Text(), "spec") == 0) {
        m_spec = Spec::FromStrDict(varList);
        m_hasSpec = true;
        return OutputAction::Handled;
    }

    if (!m_hasSpec && varList->GetVar("Client")) {
        m_spec = Spec::FromStrDict(varList);
        m_hasSpec = true;
        return OutputAction::Handled;
    }

    return OutputAction::Handled;
}

void ClientSpecInputHandler::InputData(StrBuf *buf, Error *e) {
    buf->Clear();
    buf->Append(m_formText.c_str());
    buf->Terminate();
    (void)e;
}
