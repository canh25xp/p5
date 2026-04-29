#include "error.h"

const char *error_code_name(ErrorCode code) noexcept {
    switch (code) {
    case ErrorCode::Success:
        return "Success";
    case ErrorCode::ConnectionFailed:
        return "ConnectionFailed";
    case ErrorCode::AuthenticationFailed:
        return "AuthenticationFailed";
    case ErrorCode::CommandFailed:
        return "CommandFailed";
    case ErrorCode::InvalidArguments:
        return "InvalidArguments";
    case ErrorCode::ConfigError:
        return "ConfigError";
    case ErrorCode::FileSystemError:
        return "FileSystemError";
    case ErrorCode::ApiError:
        return "ApiError";
    default:
        return "Unknown";
    }
}