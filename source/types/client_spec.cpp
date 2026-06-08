#include "types/client_spec.h"

#include "types/spec.h"

#include <iomanip>
#include <iostream>

namespace p5 {

ClientSpec::ClientSpec(const Spec &spec) {
    // Extract string fields from spec
    auto getString = [&spec](const std::string &key) -> std::string {
        auto val = spec.get(key);
        if (val && std::holds_alternative<std::string>(*val)) {
            return std::get<std::string>(*val);
        }
        return "";
    };

    client = getString("Client");
    update = getString("Update");
    access = getString("Access");
    owner = getString("Owner");
    host = getString("Host");
    description = getString("Description");
    root = getString("Root");
    options = getString("Options");
    submitOptions = getString("SubmitOptions");
    lineEnd = getString("LineEnd");
    type = getString("Type");
    backup = getString("Backup");

    auto viewVal = spec.get("View");
    if (!viewVal || !std::holds_alternative<std::vector<std::string>>(*viewVal))
        return;

    view = std::get<std::vector<std::string>>(*viewVal);
}

std::ostream &operator<<(std::ostream &os, const ClientSpec &clientSpec) {
    const size_t labelWidth = 14;

    auto printField = [&os, &labelWidth](const std::string &label, const std::string &value) {
        if (!value.empty()) {
            os << std::left << std::setw(labelWidth) << (label + ":") << value << '\n';
        }
    };

    printField("Client", clientSpec.client);
    printField("Update", clientSpec.update);
    printField("Access", clientSpec.access);
    printField("Owner", clientSpec.owner);
    printField("Host", clientSpec.host);
    printField("Description", clientSpec.description);
    printField("Root", clientSpec.root);
    printField("Options", clientSpec.options);
    printField("SubmitOptions", clientSpec.submitOptions);
    printField("LineEnd", clientSpec.lineEnd);
    printField("Type", clientSpec.type);
    printField("Backup", clientSpec.backup);

    // Print View
    os << "View:\n";
    for (const auto &line : clientSpec.view) {
        os << line << '\n';
    }

    return os;
}

} // namespace p5
