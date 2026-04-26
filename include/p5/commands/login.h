#pragma once

#include "command.h"

#include <string>

class Login : public Command {
    bool m_save{false};
    std::string m_password;

public:
    Login() : Command("login", "Login to Perforce by obtaining a session ticket") {}

    const std::string &Password() const { return m_password; }

    void Prompt(Error *err, StrBuf &rsp, int noEcho, Error *e) override {
        ClientUser::Prompt(err, rsp, noEcho, e);
        Capture(rsp, noEcho);
    }

    void Prompt(Error *err, StrBuf &rsp, int noEcho, int noOutput, Error *e) override {
        ClientUser::Prompt(err, rsp, noEcho, noOutput, e);
        Capture(rsp, noEcho);
    }

    void Prompt(const StrPtr &msg, StrBuf &rsp, int noEcho, Error *e) override {
        ClientUser::Prompt(msg, rsp, noEcho, e);
        Capture(rsp, noEcho);
    }

    void Prompt(const StrPtr &msg, StrBuf &rsp, int noEcho, int noOutput, Error *e) override {
        ClientUser::Prompt(msg, rsp, noEcho, noOutput, e);
        Capture(rsp, noEcho);
    }

    void run(const std::vector<std::string> &args) override;
    void register_cli(CLI::App &app) override;

private:
    void Capture(const StrBuf &rsp, int noEcho) {
        if (noEcho && rsp.Length() > 0)
            m_password = rsp.Text();
    }
};
