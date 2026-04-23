# Prebuilt OpenSSL for MSVC (matches FireDaemon install layout: see DEV.md / CMake presets).
# The official zip is multi-arch; p5 uses the x64/ tree (include/ + import libs under lib/).

function(p5_download_openssl_msvc_if_needed)
  if(NOT MSVC)
    return()
  endif()

  set(_P5_OSSL_VERSION "3.5.6")
  set(_P5_OSSL_BASE "${CMAKE_SOURCE_DIR}/vendor/openssl-firedaemon")
  set(_P5_OSSL_X64 "${_P5_OSSL_BASE}/x64")
  set(_P5_OSSL_ZIP_NAME "openssl-${_P5_OSSL_VERSION}.zip")
  set(_P5_OSSL_ZIP "${_P5_OSSL_BASE}/${_P5_OSSL_ZIP_NAME}")
  set(
    _P5_OSSL_URL
    "https://download.firedaemon.com/FireDaemon-OpenSSL/${_P5_OSSL_ZIP_NAME}"
  )

  if(EXISTS "${_P5_OSSL_X64}/include/openssl/opensslv.h")
    set(OPENSSL_ROOT_DIR "${_P5_OSSL_X64}" CACHE PATH
      "OpenSSL root (p5 vendor prebuild)" FORCE
    )
    set(OPENSSL_ROOT_DIR "${_P5_OSSL_X64}" PARENT_SCOPE)
    return()
  endif()

  file(MAKE_DIRECTORY "${_P5_OSSL_BASE}")
  if(NOT EXISTS "${_P5_OSSL_ZIP}")
    message(
      STATUS
      "OpenSSL not found: downloading FireDaemon OpenSSL ${_P5_OSSL_VERSION} for MSVC..."
    )
    file(
      DOWNLOAD "${_P5_OSSL_URL}" "${_P5_OSSL_ZIP}"
      SHOW_PROGRESS
      TLS_VERIFY ON
    )
  endif()

  if(NOT EXISTS "${_P5_OSSL_X64}/include/openssl/opensslv.h")
    message(STATUS "Extracting OpenSSL to ${_P5_OSSL_BASE}...")
    execute_process(
      COMMAND "${CMAKE_COMMAND}" -E tar xf "${_P5_OSSL_ZIP_NAME}"
      WORKING_DIRECTORY "${_P5_OSSL_BASE}"
      RESULT_VARIABLE _P5_OSSL_EXTRACT
    )
    if(_P5_OSSL_EXTRACT)
      message(
        FATAL_ERROR
        "Failed to extract OpenSSL. Delete ${_P5_OSSL_BASE} and try again, or set OPENSSL_ROOT_DIR to an existing install."
      )
    endif()
  endif()

  if(NOT EXISTS "${_P5_OSSL_X64}/include/openssl/opensslv.h")
    message(
      FATAL_ERROR
      "OpenSSL prebuild has unexpected layout (missing ${_P5_OSSL_X64}/include/openssl). \
Update cmake/downloadOpensslWindows.cmake (version or URL) or install OpenSSL and set OPENSSL_ROOT_DIR."
    )
  endif()

  set(OPENSSL_ROOT_DIR "${_P5_OSSL_X64}" CACHE PATH
    "OpenSSL root (p5 vendor prebuild)" FORCE
  )
  set(OPENSSL_ROOT_DIR "${_P5_OSSL_X64}" PARENT_SCOPE)
endfunction()
