# CHANGES

## [v1.1.0](https://github.com/canh25xp/p5/compare/v1.0.0...v1.1.0) (2026-06-04)

### Features

- **`p5 changes`** - New explicit command mirroring `p4 changes` flags
- **`p5 have`** - New command implementation for listing files in client workspace
- **`p5 fstat`** - New command implementation for file status information
- **`p5 filelog`** - Tagged filelog command implementation
- **`p5 frec`** -  Fast reconcile, performance improvements for reconcile operations

### Fixes

- **Windows Build** - Resolved MSVC compilation errors
- **Tabular Output** - Fixed alignment bugs on tabular rendering
- **ToLower** - Improved character case conversion using `std::tolower` with proper unsigned char handling

### Internal

- **Commands Policy System** - New policy framework for command execution
  - ClientResolve policy support
  - Command name tracking
- **Output Handler** - New OutputHandler base class
  - Command classes now extend OutputHandler
- **Progress Reporting** - ClientProgress adapters
  - Progress and TextProgress implementations
- **Resolver Support** - ClientUser resolve callbacks
  - Resolver and MergeInfo types
  - Ephemeral ClientApi with tag protocol for temp connections
- **Spec** - Validated dictionary type ported from P4Python
- **ViewMap** - Wrapper around P4API MapApi
- **Filelog Types** - Domain types ported from P4Python
- **Tag Dict** - Helpers for parallel StrDict arrays
- Refactored miscellaneous functions to `std_helper`
- Renamed `PrintSortedTsv` to `PrintSortedTable` for consistency (clients, users)
- CLI_ERROR macro now throws `CLI::RuntimeError`

## [v1.0.0](https://github.com/canh25xp/p5/compare/v0.2.0...v1.0.0) (2026-04-25)

TODO
