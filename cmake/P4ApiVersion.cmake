# Single source of truth for the bundled p4api-* directory / download version suffix.
# Override with -DP4API_VERSION=... when using a custom tree name.
if(NOT DEFINED P4API_VERSION OR P4API_VERSION STREQUAL "")
  set(P4API_VERSION "2025.1.2907437")
endif()
