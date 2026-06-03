#include "reconcile/batch_p4.h"

#include "log.h"

namespace reconcile {

namespace {

#if defined(_WIN32)
constexpr size_t kArgLengthMax = 32767 - 2048;
#else
constexpr size_t kArgLengthMax = 1024 * 1024;
#endif

void RunSlice(P5 &p5, const std::vector<std::string> &alwaysArgs, const std::vector<std::string> &fileSlice, bool useChangelist, int changelist) {
    if (alwaysArgs.empty()) {
        return;
    }
    std::vector<std::string> args(alwaysArgs.begin() + 1, alwaysArgs.end());
    if (useChangelist && changelist != 0) {
        args.push_back("-c");
        args.push_back(std::to_string(changelist));
    }
    args.insert(args.end(), fileSlice.begin(), fileSlice.end());
    p5.Run(alwaysArgs[0], args);
}

} // namespace

void RunBatched(P5 &p5, const std::vector<std::string> &alwaysArgs, const std::vector<std::string> &fileArgs, bool useChangelist, int changelist) {
    if (alwaysArgs.empty()) {
        return;
    }

    if (fileArgs.empty()) {
        RunSlice(p5, alwaysArgs, {}, useChangelist, changelist);
        return;
    }

    size_t batchStart = 0;
    size_t batchEnd = 0;
    size_t batchSize = 0;
    int batchCount = 0;

    for (size_t i = 0; i < fileArgs.size(); ++i) {
        const size_t argSize = fileArgs[i].size();
        if (batchSize + argSize > kArgLengthMax && batchEnd > batchStart) {
            RunSlice(p5, alwaysArgs, {fileArgs.begin() + static_cast<std::ptrdiff_t>(batchStart), fileArgs.begin() + static_cast<std::ptrdiff_t>(batchEnd)}, useChangelist, changelist);
            batchCount++;
            batchStart = batchEnd;
            batchSize = 0;
        }
        batchSize += argSize + 1;
        batchEnd = i + 1;
    }

    if (batchEnd > batchStart) {
        RunSlice(p5, alwaysArgs, {fileArgs.begin() + static_cast<std::ptrdiff_t>(batchStart), fileArgs.begin() + static_cast<std::ptrdiff_t>(batchEnd)}, useChangelist, changelist);
        batchCount++;
    }

    if (batchCount > 1) {
        INFO("Ran batched p4 " << alwaysArgs[0] << " in " << batchCount << " batches");
    }
}

} // namespace reconcile
