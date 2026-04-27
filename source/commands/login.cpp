#include "commands/login.h"

#include "CLI/CLI.hpp"

#include "utils/auth_store.h"
#include "log.h"
#include "p5.h"

#include <string>
#include <vector>

void Login::Prompt(Error *err, StrBuf &rsp, int noEcho, Error *e) {
    ClientUser::Prompt(err, rsp, noEcho, e);
    Capture(rsp, noEcho);
}

void Login::Prompt(Error *err, StrBuf &rsp, int noEcho, int noOutput, Error *e) {
    ClientUser::Prompt(err, rsp, noEcho, noOutput, e);
    Capture(rsp, noEcho);
}

void Login::Prompt(const StrPtr &msg, StrBuf &rsp, int noEcho, Error *e) {
    ClientUser::Prompt(msg, rsp, noEcho, e);
    Capture(rsp, noEcho);
}

void Login::Prompt(const StrPtr &msg, StrBuf &rsp, int noEcho, int noOutput, Error *e) {
    ClientUser::Prompt(msg, rsp, noEcho, noOutput, e);
    Capture(rsp, noEcho);
}

void Login::run(const std::vector<std::string> &args) {
    P5 p5;
    p5.Run("login", args, *this);
    if (!IsError()) {
        throw CLI::RuntimeError(1);
    }

    if (!m_save) {
        return;
    }

    if (Password().empty()) {
        CLI_ERROR("p5 login: --save requires an interactive password prompt");
        throw CLI::RuntimeError(1);
    }

    if (!AuthStore::SavePassword(p5.Port(), p5.User(), Password())) {
        throw CLI::RuntimeError(1);
    }
    PRINT("Password saved for " << p5.User() << "@" << p5.Port());
}

void Login::register_cli(CLI::App &app) {
    CLI::App *sub = app.add_subcommand(name, description);
    sub->prefix_command();
    sub->add_flag("--save", m_save, "Remember the entered password for future automatic login");
    sub->callback([this, sub]() { this->run(sub->remaining()); });
}
