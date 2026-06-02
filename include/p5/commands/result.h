#pragma once

#include <p4/clientapi.h>

class Result : public ClientUser {
    Error m_Error;

public:
    void HandleError(Error *e) override;

    const Error &GetError() const { return m_Error; }

    bool IsError() { return m_Error.IsError() == 1; }
    bool IsFatal() { return m_Error.IsFatal() == 1; }
};
