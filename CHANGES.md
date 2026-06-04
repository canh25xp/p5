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

### Features

- **Auto-Resolve Client** - Automatic client workspace resolution
  - Client resolver with pattern matching
  - Write resolved client to P4CONFIG
- **Logging System** - Comprehensive 4-level logging with configurable conciseness
  - Configurable concise log levels
  - Log P4 commands and arguments
  - Improved log tag formatting
- **Clients Command** - Enhanced with long version aliases for all short flags
  - Explicit arguments definition
  - Improved descriptions and variable names

### Fixes

- **Error Handling** - Fixed reversed `IsError` logic
- **CLI** - Improved error messages for unrecognized subcommands or arguments
- **CMake** - Fixed zlib_dev detection and OpenSSL root directory handling

### Internal

- **Refactoring** - Major codebase reorganization
  - Moved commands to respective source files
  - Decoupled `AutoResolveClient` from `P5` class
  - Replaced raw C strings with `std::string`
  - Organized header includes with proper prefixes
  - Removed unused utilities (`time_helpers`, `std_helpers`)
- **P5 Connection** - Refactored connection and execution methods
  - Dependency injection for connection reuse
  - Changed `P5.Run()` method signature
  - Refresh connection method improvements
- **Commands Policy** - Removed nested check error logic
  - Removed manual error logging (`CLI::SetError`)
  - Disabled default arguments injection
  - Disabled command retry by default
- **Result Type** - Added `IsFatal` method

### CI

- **CI** - Added build workflow for pull requests
- **CMake** - Changed P4 root directory check logic
