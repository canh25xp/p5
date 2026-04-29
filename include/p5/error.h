#pragma once

#include <stdexcept>
#include <string>

/// Error codes for p5 application.
/// These codes are used as exit codes when the application terminates due to an error.
enum class ErrorCode {
    Success = 0,
    ConnectionFailed = 1,
    AuthenticationFailed = 2,
    CommandFailed = 3,
    InvalidArguments = 4,
    ConfigError = 5,
    FileSystemError = 6,
    ApiError = 7,
};

/// Custom exception class for p5 errors.
/// Carries both an error code (for exit status) and a descriptive message.
class P5Error : public std::runtime_error {
public:
    /// Construct a P5Error with the given code and message.
    explicit P5Error(ErrorCode code, std::string message) : std::runtime_error(std::move(message)), m_code(code) {}

    /// Get the error code associated with this error.
    ErrorCode code() const noexcept { return m_code; }

    /// Get the numeric exit code for this error.
    int exit_code() const noexcept { return static_cast<int>(m_code); }

private:
    ErrorCode m_code;
};

/// Convert ErrorCode to a human-readable string.
const char *error_code_name(ErrorCode code) noexcept;
