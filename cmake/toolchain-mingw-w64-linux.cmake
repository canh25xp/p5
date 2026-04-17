# Cross-compile for Windows using MinGW-w64 on a Linux host (Debian/Ubuntu/WSL, etc.).
# MSVC Windows binaries are built on Windows (or Windows CI), not with this toolchain.

set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

set(CMAKE_C_COMPILER x86_64-w64-mingw32-gcc)
set(CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++)

# OpenSSL and other libraries for the Windows target live under this sysroot on typical distros.
set(MINGW_W64_LINUX_SYSROOT "/usr/x86_64-w64-mingw32" CACHE PATH "MinGW-w64 sysroot on the Linux host")

list(APPEND CMAKE_FIND_ROOT_PATH "${MINGW_W64_LINUX_SYSROOT}")
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

# Default OpenSSL layout on Debian/Ubuntu mingw-w64 packages (override with -DOPENSSL_ROOT_DIR=...).
set(OPENSSL_ROOT_DIR "${MINGW_W64_LINUX_SYSROOT}" CACHE PATH "OpenSSL root for MinGW-w64 cross on Linux")
