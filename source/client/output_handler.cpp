#include "client/output_handler.h"

#include "client/progress.h"
#include "client/resolver.h"

#include <iostream>
#include <p4/clientmerge.h>

namespace {

MergeInfo MergeInfoFromStatus(MergeStatus status) {
    MergeInfo info;
    switch (status) {
    case CMS_SKIP:
        info.mergeHint = "s";
        break;
    case CMS_MERGED:
        info.mergeHint = "m";
        break;
    case CMS_EDIT:
        info.mergeHint = "e";
        break;
    case CMS_THEIRS:
        info.mergeHint = "t";
        break;
    case CMS_YOURS:
        info.mergeHint = "y";
        break;
    case CMS_QUIT:
        info.mergeHint = "q";
        break;
    default:
        info.mergeHint = "s";
        break;
    }
    return info;
}

MergeStatus MergeStatusFromHint(const std::string &hint) {
    if (hint.empty()) {
        return CMS_SKIP;
    }
    switch (hint[0]) {
    case 's':
        return CMS_SKIP;
    case 'm':
        return CMS_MERGED;
    case 'e':
        return CMS_EDIT;
    case 't':
        return CMS_THEIRS;
    case 'y':
        return CMS_YOURS;
    case 'q':
        return CMS_QUIT;
    default:
        return CMS_SKIP;
    }
}

} // namespace

OutputAction OutputHandler::onOutputText(const char * /*data*/, int /*length*/) {
    return OutputAction::Report;
}

OutputAction OutputHandler::onOutputBinary(const char * /*data*/, int /*length*/) {
    return OutputAction::Report;
}

OutputAction OutputHandler::onOutputStat(StrDict * /*varList*/) {
    return OutputAction::Report;
}

OutputAction OutputHandler::onOutputInfo(char /*level*/, const char * /*data*/) {
    return OutputAction::Report;
}

OutputAction OutputHandler::onOutputMessage(Error * /*err*/) {
    return OutputAction::Report;
}

void OutputHandler::OutputText(const char *data, int length) {
    switch (onOutputText(data, length)) {
    case OutputAction::Cancel:
        return;
    case OutputAction::Handled:
        return;
    case OutputAction::Report:
    default:
        ClientUser::OutputText(data, length);
        break;
    }
}

void OutputHandler::OutputBinary(const char *data, int length) {
    switch (onOutputBinary(data, length)) {
    case OutputAction::Cancel:
        return;
    case OutputAction::Handled:
        return;
    case OutputAction::Report:
    default:
        ClientUser::OutputBinary(data, length);
        break;
    }
}

void OutputHandler::OutputStat(StrDict *varList) {
    if (onOutputStat(varList) == OutputAction::Report) {
        ClientUser::OutputStat(varList);
    }
}

void OutputHandler::OutputInfo(char level, const char *data) {
    switch (onOutputInfo(level, data)) {
    case OutputAction::Cancel:
        return;
    case OutputAction::Handled:
        return;
    case OutputAction::Report:
    default:
        ClientUser::OutputInfo(level, data);
        break;
    }
}

void OutputHandler::Message(Error *err) {
    if (onOutputMessage(err) == OutputAction::Report) {
        ClientUser::Message(err);
    }
}

int OutputHandler::Resolve(ClientMerge *merge, Error *error) {
    if (m_resolver && merge) {
        MergeInfo info = MergeInfoFromStatus(merge->AutoResolve(CMF_AUTO));
        const std::string hint = m_resolver->resolve(info);
        return static_cast<int>(MergeStatusFromHint(hint));
    }
    return ClientUser::Resolve(merge, error);
}

int OutputHandler::Resolve(ClientResolveA *resolve, int preview, Error *error) {
    if (m_resolver && resolve) {
        MergeInfo info = MergeInfoFromStatus(resolve->AutoResolve(CMF_AUTO));
        const std::string hint = m_resolver->actionResolve(info);
        return static_cast<int>(MergeStatusFromHint(hint));
    }
    return ClientUser::Resolve(resolve, preview, error);
}

ClientProgress *OutputHandler::CreateProgress(int progressType) {
    if (m_progress) {
        m_progress->init(progressType);
        if (!m_progressAdapter) {
            m_progressAdapter = new ProgressClientAdapter(m_progress);
        }
        return m_progressAdapter;
    }
    return ClientUser::CreateProgress(progressType);
}

ClientProgress *OutputHandler::CreateProgress(int progressType, P4INT64 fileSize) {
    (void)fileSize;
    return CreateProgress(progressType);
}

OutputHandler::~OutputHandler() {
    delete m_progressAdapter;
}

OutputAction ReportHandler::onOutputText(const char *data, int length) {
    std::cout << "text: ";
    std::cout.write(data, length);
    std::cout << '\n';
    return OutputAction::Handled;
}

OutputAction ReportHandler::onOutputBinary(const char *data, int length) {
    std::cout << "binary: ";
    std::cout.write(data, length);
    std::cout << '\n';
    return OutputAction::Handled;
}

OutputAction ReportHandler::onOutputStat(StrDict *varList) {
    if (varList) {
        StrRef var;
        StrRef val;
        for (int i = 0; varList->GetVar(i, var, val); ++i) {
            std::cout << "stat: " << var.Text() << " " << val.Text() << '\n';
        }
    }
    return OutputAction::Handled;
}

OutputAction ReportHandler::onOutputInfo(char level, const char *data) {
    std::cout << "info(" << level << "): " << data << '\n';
    return OutputAction::Handled;
}

OutputAction ReportHandler::onOutputMessage(Error *err) {
    if (err) {
        StrBuf msg;
        err->Fmt(&msg);
        std::cout << "error: " << msg.Text() << '\n';
    }
    return OutputAction::Handled;
}
