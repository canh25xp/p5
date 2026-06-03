#pragma once

#include <string>
#include <vector>

namespace reconcile {

struct ReconcilePlan {
    std::vector<std::string> doAdd;
    std::vector<std::string> doEdit;
    std::vector<std::string> doReopenEdit;
    std::vector<std::string> doDelete;
    std::vector<std::string> doReopenDelete;
    std::vector<std::string> doRevertAdd;
    std::vector<std::string> doRevertEdit;
    std::vector<std::string> doRevertDelete;
    std::vector<std::string> unsupportedDepotFiles;
};

struct ReconcileOptions {
    bool preview{false};
    bool edit{true};
    bool add{true};
    bool deleteFiles{true};
    bool mtimeOptimize{false};
    bool skipIgnore{false};
    int changelist{0};
};

} // namespace reconcile
