#include "commands/result.h"

#include <p4/clientapi.h>

#include "log.h"

void Result::HandleError(Error *e) {
    StrBuf str;
    e->Fmt(&str);
    // TODO: handle auto login here
    // Received error: error Your session has expired, please login again.
    // Received error: error Perforce password (P4PASSWD) invalid or unset.
    API_ERROR("Received error: " << e->FmtSeverity() << " " << str.Text());
    m_Error = *e;
}
