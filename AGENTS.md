# P5 development context

## What this project is

`p5` is a command-line tool meant to replace the `p4` CLI. It adds features `p4` lacks and improves existing behavior, visually and functionally.

## Build

Prefer the workflow preset (generate + build in one step):

```sh
cmake --workflow default
```

```sh
./build/Debug/p5 --help
```

Key details the build depends on:

- Uses CMake presets (`CMakePresets.json`). Default = Ninja Debug at `build/Debug`.
- Release build: `cmake --workflow ninja-release` → `build/Release/p5`
- CI sets `P5_BINARY_CACHE` to cache downloaded dependencies (P4 API, OpenSSL). If builds are slow or re-downloading, set this locally.
- Submodules are required (`vendor/CLI11`). Always clone with `--recurse-submodules` or run `git submodule update --init --recursive`.

Install shortcut:

```sh
./scripts/install.sh        # installs Release to ~/.local/bin
./scripts/install.sh -d     # installs Debug
```

## Tests

- No CTest or unified test runner.
- Example test (`tests/test_log.cpp`) is standalone and compiled manually (see comment in file).
- If adding tests, follow this pattern or wire into CMake explicitly (not currently done).

## Command-line implementation

Parsing and subcommands use **CLI11**. Reference docs live under `vendor/CLI11/book/`.

Important structure:

- Entry point: `source/main.cpp`
- CLI wiring: `source/options.cpp`, `source/commands.cpp`
- Subcommands: `source/commands/*` with headers in `include/p5/commands/*`
- Utilities: `source/utils/*`

When adding commands:

- Mirror `p4` command names and flags exactly unless there is a strong reason not to.
- Follow existing command file pattern (one file per command, registered in `commands.cpp`).

## Formatting

- Use `scripts/format_cpp.sh` (clang-format over all tracked C/C++ files).
- Excludes `vendor/` automatically.

## Dependencies / Tooling quirks

- Project downloads P4 API and OpenSSL via CMake (`cmake/download*.cmake`). Network access may be required on first build.
- Linux CI requires: `cmake`, `ninja`, `libssl-dev`, `libzstd-dev`.
- Windows builds use MSVC presets (`msvc2022`).

## Compatibility constraint (critical)

- This tool is a drop-in `p4` replacement. Behavioral and CLI compatibility is a primary requirement.
- Do not introduce breaking flag/argument changes without strong justification.
