cmake_minimum_required(VERSION 3.15)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(P4ApiVersion)
include(downloadP4api)
include(Submodules)

set(P5_VERSION_MAJOR 0)
set(P5_VERSION_MINOR 2)
set(P5_VERSION_PATCH 0)
set(P5_VERSION "${P5_VERSION_MAJOR}.${P5_VERSION_MINOR}.${P5_VERSION_PATCH}")

project(
    p5
    LANGUAGES CXX
    VERSION ${P5_VERSION}
    DESCRIPTION "p5: Complementary p4 command-line tools"
)

# When OFF, dbg is still disabled for Release-like configs via a generator expression below.
option(DBG_MACRO_DISABLE "Force-disable dbg macro in all configurations (including Debug)" OFF)
option(DBG_MACRO_NO_WARNING "disable debug macro warning" TRUE)

# PRINT/INFO/WARN are compiled in when CMAKE_BUILD_TYPE/Debug config is Debug, or when this is ON (e.g. Release + diagnostics).
option(P5_VERBOSE_LOGGING "Enable PRINT/INFO/WARN log macros in all configs (Release, etc.)" OFF)
message(STATUS "P5_VERBOSE_LOGGING:" ${P5_VERBOSE_LOGGING})

# Work-arround cmake error:
# Compatibility with CMake < 3.5 has been removed from CMake.
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)

# Generate compile_commands.json, will be used by lsp_server
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT P4API_ROOT_DIR)
  set(P4API_VENDOR_DIR "${CMAKE_SOURCE_DIR}/vendor/p4api")
  # TODO: prepend vs2017_static when build for windows, like:
  # p4api-${P4API_VERSION}-vs2017_static
  # p4api-${P4API_VERSION}-vs2022_static
  set(P4API_ROOT_DIR "${P4API_VENDOR_DIR}/p4api-${P4API_VERSION}")
else()
  if(NOT DEFINED P4API_VERSION OR P4API_VERSION STREQUAL "")
    get_filename_component(_p4api_dir_name "${P4API_ROOT_DIR}" NAME)
    if(_p4api_dir_name MATCHES "^p4api-(.+)$")
      set(P4API_VERSION "${CMAKE_MATCH_1}")
    else()
      set(P4API_VERSION "unknown")
    endif()
  endif()
endif()

message(STATUS "P4API_ROOT_DIR:" ${P4API_ROOT_DIR})
message(STATUS "P4API_VERSION:" ${P4API_VERSION})

if(MSVC)
  if(CMAKE_GENERATOR MATCHES "Visual Studio 17 2022")
    set(_P4API_RELEASE_DIR "${P4API_VENDOR_DIR}/p4api-${P4API_VERSION}-vs2022_static")
    set(_P4API_DEBUG_DIR   "${P4API_VENDOR_DIR}/p4api-${P4API_VERSION}-vs2022_static_vsdebug")
  elseif(CMAKE_GENERATOR MATCHES "Visual Studio 15 2017")
    set(_P4API_RELEASE_DIR "${P4API_VENDOR_DIR}/p4api-${P4API_VERSION}-vs2017_static")
    set(_P4API_DEBUG_DIR   "${P4API_VENDOR_DIR}/p4api-${P4API_VERSION}-vs2017_static_vsdebug")
  endif()
  if(NOT EXISTS "${_P4API_RELEASE_DIR}" OR NOT EXISTS "${_P4API_DEBUG_DIR}")
    download_and_extract_p4api()
  endif()
else()
  if(NOT EXISTS "${P4API_ROOT_DIR}")
    download_and_extract_p4api()
  endif()
endif()

file(GLOB_RECURSE INCLUDES include/**.h)
file(GLOB_RECURSE SOURCES source/**.cpp)
add_executable(${PROJECT_NAME} ${SOURCES} ${INCLUDES})

# CLI description / --version (PROJECT_* and P4API_VERSION from above)
target_compile_definitions(
  ${PROJECT_NAME}
  PRIVATE
    "P5_APP_DESCRIPTION=\"${PROJECT_DESCRIPTION}\""
    "P5_VERSION_MAJOR=\"${P5_VERSION_MAJOR}\""
    "P5_VERSION_MINOR=\"${P5_VERSION_MINOR}\""
    "P5_VERSION_PATCH=\"${P5_VERSION_PATCH}\""
    "P5_VERSION=\"${P5_VERSION}\""
    "P5_VERSION_STRING=\"p5 version ${PROJECT_VERSION} (p4api version ${P4API_VERSION})\""
)

# Auto-initialize git submodules if needed (e.g., repo cloned without --recursive)
init_git_submodules()

add_subdirectory(vendor)
add_subdirectory(tests)

target_include_directories(${PROJECT_NAME} PRIVATE
    include/p5
    vendor/dbg-macro
)

if(MSVC)
  # NOTE: I'm not sure if these lines below are the same, so better to keep all
  # add_compile_options("/MT$<$<CONFIG:Debug>:d>")
  # set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
  # Compile with /MT flag for release and /MTd for debug.
  # Do not use /MD or /MDd
  set_property(TARGET ${PROJECT_NAME} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE P4API::client CLI11::CLI11)

if(DBG_MACRO_DISABLE)
  target_compile_definitions(${PROJECT_NAME} PRIVATE DBG_MACRO_DISABLE)
else()
  # Release / RelWithDebInfo / MinSizeRel: no dbg(); Debug keeps dbg() unless DBG_MACRO_DISABLE is ON.
  target_compile_definitions(${PROJECT_NAME} PRIVATE $<$<NOT:$<CONFIG:Debug>>:DBG_MACRO_DISABLE>)
endif()

if(DBG_MACRO_NO_WARNING)
  target_compile_definitions(${PROJECT_NAME} PRIVATE DBG_MACRO_NO_WARNING)
endif()

target_compile_definitions(${PROJECT_NAME} PRIVATE $<$<OR:$<CONFIG:Debug>,$<BOOL:${P5_VERBOSE_LOGGING}>>:P5_VERBOSE_LOGGING=1>)

# TODO: exclude `include/dbg.h` from install
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
