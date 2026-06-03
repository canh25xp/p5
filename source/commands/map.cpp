#include <CLI/CLI.hpp>

#include "commands/map.h"
#include "commands.h"
#include "client/client_spec.h"
#include "log.h"
#include "options.h"
#include "types/spec.h"
#include "types/view_map.h"

#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

struct Position {
    enum class Kind { First, Last, Index } kind;
    int index{0}; // 1-based when kind == Index
};

std::string resolveClientName() {
    const std::string &client = g_options.client();
    if (client.empty()) {
        return {};
    }
    return client;
}

void printViewList(const std::string &clientName, const ViewMap &view) {
    const std::vector<std::string> lines = view.lines();
    if (lines.empty()) {
        std::cout << "map: client " << clientName << " has no View mappings\n";
        return;
    }
    for (std::size_t i = 0; i < lines.size(); ++i) {
        std::cout << (i + 1) << ": " << lines[i] << '\n';
    }
}

std::string prepareMappingLine(const std::vector<std::string> &args, const std::string &clientName) {
    const std::string joined = ViewMap::joinMappingArgs(args);
    if (joined.empty()) {
        throw std::runtime_error("mapping argument required");
    }

    std::string err;

    if (args.size() == 1 && args[0].compare(0, 2, "//") == 0 && args[0].find(' ') == std::string::npos) {
        std::string line;
        if (!ViewMap::expandQuickAdd(args[0], clientName, line, err)) {
            throw std::runtime_error(err);
        }
        return line;
    }

    if (!ViewMap::validateLine(joined, err)) {
        throw std::runtime_error(err);
    }
    return joined;
}

Position parsePosition(const bool first, const bool last, const int index) {
    int set = 0;
    if (first) {
        ++set;
    }
    if (last) {
        ++set;
    }
    if (index > 0) {
        ++set;
    }
    if (set != 1) {
        throw std::runtime_error("specify exactly one of --first, --last, or -i/--index");
    }
    if (first) {
        return {Position::Kind::First, 1};
    }
    if (last) {
        return {Position::Kind::Last, 0};
    }
    return {Position::Kind::Index, index};
}

std::size_t toZeroBasedIndex(const Position &pos, const std::size_t count) {
    switch (pos.kind) {
    case Position::Kind::First:
        if (count == 0) {
            throw std::runtime_error("view is empty");
        }
        return 0;
    case Position::Kind::Last:
        if (count == 0) {
            throw std::runtime_error("view is empty");
        }
        return count - 1;
    case Position::Kind::Index:
        if (pos.index < 1 || static_cast<std::size_t>(pos.index) > count) {
            throw std::runtime_error("index out of range");
        }
        return static_cast<std::size_t>(pos.index - 1);
    }
    throw std::runtime_error("invalid position");
}

std::size_t resolveInsertIndex(const Position &pos, const std::size_t count) {
    switch (pos.kind) {
    case Position::Kind::First:
        return 0;
    case Position::Kind::Last:
        return count;
    case Position::Kind::Index:
        if (pos.index < 1 || static_cast<std::size_t>(pos.index) > count + 1) {
            throw std::runtime_error("index out of range for insert");
        }
        return static_cast<std::size_t>(pos.index - 1);
    }
    throw std::runtime_error("invalid position");
}

} // namespace

void Map::run(const std::vector<std::string> & /*args*/) {
    g_options.set_command(name);
    CLI_ERROR("map: use a subcommand (list, add, insert, edit, delete)");
    throw CLI::RuntimeError(1);
}

void Map::register_cli(CLI::App &app) {
    auto *sub = app.add_subcommand(name, description);

    auto runList = [this]() {
        g_options.set_command(name);
        const std::string clientName = resolveClientName();
        if (clientName.empty()) {
            CLI_ERROR("map: P4CLIENT is not set (use -c or set P4CLIENT)");
            throw CLI::RuntimeError(1);
        }

        P5 &p5 = m_commands->p5();
        Spec spec;
        std::string err;
        if (!p5::client_spec::Fetch(p5, clientName, spec, err)) {
            CLI_ERROR("map: " << err);
            throw CLI::RuntimeError(1);
        }
        if (HasNonEmptyStream(spec)) {
            CLI_ERROR("map: client has a Stream; view is managed by the stream (use p4 client)");
            throw CLI::RuntimeError(1);
        }

        printViewList(clientName, ViewMap::fromLines(GetViewLines(spec)));
    };

    sub->add_subcommand("list", "List view mappings with line numbers")->callback(runList);

    auto mutate = [this](auto &&mutator) {
        g_options.set_command(name);
        const std::string clientName = resolveClientName();
        if (clientName.empty()) {
            CLI_ERROR("map: P4CLIENT is not set (use -c or set P4CLIENT)");
            throw CLI::RuntimeError(1);
        }

        P5 &p5 = m_commands->p5();
        Spec spec;
        std::string err;
        if (!p5::client_spec::Fetch(p5, clientName, spec, err)) {
            CLI_ERROR("map: " << err);
            throw CLI::RuntimeError(1);
        }
        if (HasNonEmptyStream(spec)) {
            CLI_ERROR("map: client has a Stream; view is managed by the stream (use p4 client)");
            throw CLI::RuntimeError(1);
        }

        ViewMap view = ViewMap::fromLines(GetViewLines(spec));
        try {
            mutator(view, clientName);
        } catch (const std::exception &ex) {
            CLI_ERROR("map: " << ex.what());
            throw CLI::RuntimeError(1);
        }

        SetViewLines(spec, view.lines());
        if (!p5::client_spec::Save(p5, spec, err)) {
            CLI_ERROR("map: " << err);
            throw CLI::RuntimeError(1);
        }

        std::cout << "map: client " << clientName << " view now " << view.size() << " line(s)\n";
    };

    auto *addCmd = sub->add_subcommand("add", "Append a view mapping and save");
    addCmd->allow_extras();
    addCmd->callback([this, addCmd, mutate]() {
        const std::vector<std::string> mappingArgs = addCmd->remaining();
        mutate([&](ViewMap &view, const std::string &clientName) {
            const std::string line = prepareMappingLine(mappingArgs, clientName);
            view.append(line);
        });
    });

    auto *insertCmd = sub->add_subcommand("insert", "Insert a view mapping at a position");
    bool insertFirst = false;
    bool insertLast = false;
    int insertIndex = 0;
    insertCmd->add_flag("--first", insertFirst, "Insert at the beginning");
    insertCmd->add_flag("--last", insertLast, "Insert at the end (default when no position flag is given)");
    insertCmd->add_option("-i,--index", insertIndex, "Insert before 1-based line number")->check(CLI::PositiveNumber);
    insertCmd->allow_extras();
    insertCmd->callback([this, insertCmd, mutate, insertFirst, insertLast, insertIndex]() {
        const std::vector<std::string> mappingArgs = insertCmd->remaining();
        bool first = insertFirst;
        bool last = insertLast;
        int index = insertIndex;
        if (!first && !last && index == 0) {
            last = true;
        }
        const Position pos = parsePosition(first, last, index);
        mutate([&](ViewMap &view, const std::string &clientName) {
            const std::string line = prepareMappingLine(mappingArgs, clientName);
            view.insertAt(resolveInsertIndex(pos, view.size()), line);
        });
    });

    auto *editCmd = sub->add_subcommand("edit", "Replace a view mapping at a line number");
    int editIndex = 0;
    editCmd->add_option("-i,--index", editIndex, "1-based line number to replace")->required()->check(CLI::PositiveNumber);
    editCmd->allow_extras();
    editCmd->callback([this, editCmd, mutate, editIndex]() {
        const std::vector<std::string> mappingArgs = editCmd->remaining();
        const Position pos = parsePosition(false, false, editIndex);
        mutate([&](ViewMap &view, const std::string &clientName) {
            const std::string line = prepareMappingLine(mappingArgs, clientName);
            view.replaceAt(toZeroBasedIndex(pos, view.size()), line);
        });
    });

    auto *deleteCmd = sub->add_subcommand("delete", "Remove a view mapping");
    bool deleteFirst = false;
    bool deleteLast = false;
    int deleteIndex = 0;
    deleteCmd->add_flag("--first", deleteFirst, "Delete the first mapping");
    deleteCmd->add_flag("--last", deleteLast, "Delete the last mapping");
    deleteCmd->add_option("-i,--index", deleteIndex, "Delete 1-based line number")->check(CLI::PositiveNumber);
    deleteCmd->callback([mutate, deleteFirst, deleteLast, deleteIndex]() {
        bool first = deleteFirst;
        bool last = deleteLast;
        int index = deleteIndex;
        if (!first && !last && index == 0) {
            CLI_ERROR("map delete: specify --first, --last, or -i/--index");
            throw CLI::RuntimeError(1);
        }
        const Position pos = parsePosition(first, last, index);
        mutate([&](ViewMap &view, const std::string & /*clientName*/) { view.eraseAt(toZeroBasedIndex(pos, view.size())); });
    });

    // `p5 map` with no subcommand lists mappings (same as `p5 map list`).
    sub->callback(runList);
}
