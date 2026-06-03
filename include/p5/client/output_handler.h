#pragma once

#include "commands/result.h"

class StrDict;

enum class OutputAction {
    Report = 0,
    Handled = 1,
    Cancel = 2,
};

/// OutputHandler — hooks for ClientUser output callbacks.
class OutputHandler : public Result {
public:
    void setResolver(class Resolver *resolver) { m_resolver = resolver; }
    void setProgress(class Progress *progress) { m_progress = progress; }

    void OutputText(const char *data, int length) override;
    void OutputBinary(const char *data, int length) override;
    void OutputStat(StrDict *varList) override;
    void OutputInfo(char level, const char *data) override;
    void Message(Error *err) override;

    int Resolve(ClientMerge *merge, Error *error) override;
    int Resolve(ClientResolveA *resolve, int preview, Error *error) override;

    ClientProgress *CreateProgress(int progressType) override;
    ClientProgress *CreateProgress(int progressType, P4INT64 fileSize) override;

    ~OutputHandler() override;

protected:
    virtual OutputAction onOutputText(const char *data, int length);
    virtual OutputAction onOutputBinary(const char *data, int length);
    virtual OutputAction onOutputStat(StrDict *varList);
    virtual OutputAction onOutputInfo(char level, const char *data);
    virtual OutputAction onOutputMessage(Error *err);

    class Resolver *m_resolver{nullptr};
    class Progress *m_progress{nullptr};

private:
    class ProgressClientAdapter *m_progressAdapter{nullptr};
};

/// Debug handler that prints all tagged output (P4Python ReportHandler).
class ReportHandler : public OutputHandler {
protected:
    OutputAction onOutputText(const char *data, int length) override;
    OutputAction onOutputBinary(const char *data, int length) override;
    OutputAction onOutputStat(StrDict *varList) override;
    OutputAction onOutputInfo(char level, const char *data) override;
    OutputAction onOutputMessage(Error *err) override;
};
