#include "types/client_spec.h"

#include "types/spec.h"
#include "types/view_map.h"

#include <iomanip>
#include <iostream>

namespace p5 {

ClientSpec ParseClientSpec(const Spec &spec) {
    ClientSpec clientSpec;

    // Extract string fields from spec
    auto getString = [&spec](const std::string &key) -> std::string {
        auto val = spec.get(key);
        if (val && std::holds_alternative<std::string>(*val)) {
            return std::get<std::string>(*val);
        }
        return "";
    };

    clientSpec.client = getString("Client");
    clientSpec.update = getString("Update");
    clientSpec.access = getString("Access");
    clientSpec.owner = getString("Owner");
    clientSpec.host = getString("Host");
    clientSpec.description = getString("Description");
    clientSpec.root = getString("Root");
    clientSpec.options = getString("Options");
    clientSpec.submitOptions = getString("SubmitOptions");
    clientSpec.lineEnd = getString("LineEnd");
    clientSpec.type = getString("Type");
    clientSpec.backup = getString("Backup");

    // Extract View as ViewMap
    auto viewVal = spec.get("View");
    if (viewVal && std::holds_alternative<std::vector<std::string>>(*viewVal)) {
        const auto &viewLines = std::get<std::vector<std::string>>(*viewVal);
        clientSpec.view.InsertTranslationMapping(viewLines);
    }

    return clientSpec;
}

void ClientSpec::Print() const {
    const size_t labelWidth = 14;

    // TODO: replace with PrintTable method from tabular_renderer
    auto printField = [&labelWidth](const std::string &label, const std::string &value) {
        if (!value.empty()) {
            std::cout << std::left << std::setw(labelWidth) << (label + ":") << value << '\n';
        }
    };

    printField("Client", client);
    printField("Update", update);
    printField("Access", access);
    printField("Owner", owner);
    printField("Host", host);
    printField("Description", description);
    printField("Root", root);
    printField("Options", options);
    printField("SubmitOptions", submitOptions);
    printField("LineEnd", lineEnd);
    printField("Type", type);
    printField("Backup", backup);

    // Print View
    if (!view.isEmpty()) {
        std::cout << "View:\n";
        for (const auto &line : view.asArray()) {
            std::cout << "    " << line << '\n';
        }
    }
}

} // namespace p5
