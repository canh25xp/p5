# P5 Architectural Review

## 1. Architecture Overview

### Current Architectural Style

The project follows a **modular layered architecture** with the following characteristics:

```
┌─────────────────────────────────────────────────────────────┐
│                    CLI Layer (CLI11)                        │
│  main.cpp → Commands → Command subclasses                   │
├─────────────────────────────────────────────────────────────┤
│                    Application Layer                        │
│  P5 (API wrapper) → Options → Log                           │
├─────────────────────────────────────────────────────────────┤
│                    Integration Layer                        │
│  ClientApi (P4API) → Result/ClientUser                      │
├─────────────────────────────────────────────────────────────┤
│                    Utility Layer                            │
│  ClientResolver, STDHelpers, TimeHelpers, CLIHelpers        │
└─────────────────────────────────────────────────────────────┘
```

### Major Components and Responsibilities

| Component               | File(s)                   | Responsibility                            |
| ----------------------- | ------------------------- | ----------------------------------------- |
| `main.cpp`              | Entry point               | CLI initialization, command registration  |
| `Commands`              | commands.cpp/h            | Batch CLI11 subcommand definitions        |
| `Command`               | command.h                 | Base class for individual commands        |
| `P5`                    | p5.cpp/h                  | P4API wrapper, connection lifecycle       |
| `Options`               | options.cpp/h             | Global CLI flags and environment          |
| `Result`                | result.cpp/h              | P4API output handler (extends ClientUser) |
| `Clients`/`Users`/`Set` | commands/\*.cpp           | Concrete command implementations          |
| `Log`                   | log.cpp/h                 | Logging infrastructure                    |
| `ClientResolver`        | utils/client_resolver.cpp | Auto-resolve client from CWD              |

### Data and Control Flow

```
User Input (argv)
    │
    ▼
┌─────────────┐
│ CLI11 Parse │◄── Options (global flags)
└─────────────┘
    │
    ▼
┌─────────────┐
│ Command::run│
└─────────────┘
    │
    ▼
┌─────────────┐
│    P5       │◄── Options (connection params)
│ (creates)   │
└─────────────┘
    │
    ▼
┌─────────────┐
│  ClientApi  │ (P4API)
└─────────────┘
    │
    ▼
┌─────────────┐
│   Result    │ (collects output)
└─────────────┘
    │
    ▼
  stdout/stderr
```

---

## 2. Code Structure Evaluation

### Directory Organization

**Current structure:**

```
p5/
├── include/p5/           # Public headers (good)
│   ├── commands/         # Command-specific headers
│   └── utils/            # Utility headers
├── source/               # Implementation files
│   ├── commands/
│   └── utils/
├── tests/                # Tests (minimal)
├── cmake/                # Build scripts
├── docs/                 # Documentation
└── vendor/               # Third-party deps
```

**Assessment:**

- ✅ Clear separation between headers and implementation
- ✅ Logical grouping by functionality (commands, utils)
- ⚠️ `include/p5` contains both public headers and internal utilities
- ⚠️ No `src` vs `include` convention for internal vs public API

### Naming Conventions

| Aspect    | Convention            | Issues                                 |
| --------- | --------------------- | -------------------------------------- |
| Classes   | PascalCase            | Consistent ✅                          |
| Members   | m_Prefix + PascalCase | Inconsistent: `m_Clients` vs `m_Users` |
| Functions | camelCase             | Consistent ✅                          |
| Constants | kPrefix + PascalCase  | Mixed with `COMMAND_RETRIES` (no k)    |
| Files     | snake_case            | Consistent ✅                          |

### Separation of Concerns

| Concern           | Handling                         | Issues                |
| ----------------- | -------------------------------- | --------------------- |
| CLI parsing       | CLI11 in `Command::register_cli` | Clean ✅              |
| Business logic    | In `Command::run`                | Mixed with I/O ⚠️     |
| P4API integration | `P5` and `Result`                | Clean ✅              |
| Output formatting | In command classes               | Could be extracted ⚠️ |
| Error handling    | `Error` from P4API + macros      | Inconsistent ⚠️       |

---

## 3. Dependency Analysis

### Tight Coupling Issues

#### 1. Global Options Object

**Problem:** `g_options` is a global variable accessed from multiple translation units.

```cpp
// options.h
extern Options g_options;

// p5.cpp
m_ClientAPI.SetPort(g_options.port().c_str());
m_ClientAPI.SetUser(g_options.user().c_str());

// main.cpp
g_options.add(app);
```

**Impact:** Makes testing difficult, hides dependencies, violates DI principles.

#### 2. P5 and P4API Tight Coupling

**Problem:** `P5` directly embeds `ClientApi` and handles all P4API interactions.

```cpp
class P5 {
    ClientApi m_ClientAPI;  // Direct composition
    // ...
};
```

**Impact:** Cannot mock P4API for testing, cannot have different implementations.

#### 3. Command Depends on Concrete P5

**Problem:** Commands create `P5` instances directly.

```cpp
void Clients::run(const std::vector<std::string> &args) {
    P5 p5;  // Direct instantiation
    Clients r = p5.ListClients(args);
}
```

**Impact:** Commands cannot be tested without real P4 server connection.

### Cyclic Dependencies

**No direct cycles detected**, but there are concerning indirect relationships:

```
Command → Result → (P4API)
    ↓
   P5 → Options (global) → Enviro (P4API)
```

### Dependency Inversion Opportunities

1. **Introduce IP4Client interface:**

```cpp
// Proposed
class IP4Client {
public:
    virtual ~IP4Client() = default;
    virtual Result Run(const char* command, int argc, char** argv) = 0;
    virtual void SetProtocol(const char* key, const char* value) = 0;
};

class P5 : public IP4Client { /* ... */ };
class MockP5 : public IP4Client { /* ... */ };  // For testing
```

2. **Inject Options into P5:**

```cpp
// Current
P5::P5() {
    m_ClientAPI.SetPort(g_options.port().c_str());  // Global access
}

// Proposed
P5::P5(const Options& options) {
    m_ClientAPI.SetPort(options.port().c_str());  // Dependency injection
}
```

---

## 4. Design & Patterns

### Existing Patterns

| Pattern             | Location                    | Assessment              |
| ------------------- | --------------------------- | ----------------------- |
| **Command**         | `Command` class             | ✅ Properly implemented |
| **Template Method** | `Command::run` virtual      | ✅ Correctly used       |
| **RAII**            | `P5` constructor/destructor | ⚠️ Partial - see issues |
| **Facade**          | `P5` wraps P4API            | ✅ Good abstraction     |

### Missing Patterns

#### 1. Factory Pattern for Commands

**Problem:** Commands manually registered with different overloads.

```cpp
// main.cpp - repetitive manual registration
commands.add("add", "Open a new file to add it to the depot");
commands.add({"change", "changelist"}, "Create or edit a changelist description");
commands.add(std::make_unique<Clients>());
commands.add(std::make_unique<Users>());
commands.add(std::make_unique<Set>(g_options));
```

**Recommendation:** Use a command factory or auto-registration.

```cpp
// Proposed: Auto-registration via static initialization
struct CommandRegistrar {
    CommandRegistrar(std::unique_ptr<Command> cmd) {
        CommandRegistry::instance().register_command(std::move(cmd));
    }
};

#define REGISTER_COMMAND(cls) \
    static CommandRegistrar _reg_##cls(std::make_unique<cls>());
```

#### 2. Strategy Pattern for Output Formatting

**Problem:** Output formatting mixed with command logic.

```cpp
void Clients::run(const std::vector<std::string> &args) {
    // ... business logic ...
    for (const ClientName &name : names) {
        PRINT(name);
        PRINT("  root " << data.root);
        // ... formatting ...
    }
}
```

**Recommendation:** Extract output formatting.

```cpp
// Proposed
class OutputFormatter {
public:
    virtual void format(const Clients::ClientMap& clients, std::ostream& out) = 0;
};

class TableFormatter : public OutputFormatter { /* ... */ };
class JsonFormatter : public OutputFormatter { /* ... */ };
```

#### 3. Builder Pattern for Complex Configuration

**Problem:** `P5` initialization is complex with multiple steps.

```cpp
// p5.cpp
bool P5::Initialize() {
    m_ClientAPI.SetPort(g_options.port().c_str());
    m_ClientAPI.SetUser(g_options.user().c_str());
    m_ClientAPI.SetClient(g_options.client().c_str());
    for (const auto &proto : g_options.p4Protocol()) {
        m_ClientAPI.SetProtocol(proto.first.c_str(), proto.second.c_str());
    }
    // ... more setup ...
}
```

### Misused Patterns

#### Inheritance for Code Reuse (Command extends Result)

**Problem:** `Command` inherits from `Result` for convenience, not "is-a" relationship.

```cpp
class Command : public Result {  // ❌ Not a proper "is-a"
    // Command is NOT a Result, it USES a Result
};
```

**Recommendation:** Use composition.

```cpp
class Command {
protected:
    std::unique_ptr<Result> m_result;  // Composition
};
```

---

## 5. Modern C++ Usage

### Current C++ Standard: C++17

### Smart Pointers

| Usage                                  | Assessment        |
| -------------------------------------- | ----------------- |
| `std::unique_ptr<Command>` in Commands | ✅ Correct        |
| `std::unique_ptr<Enviro>` in Options   | ✅ Correct        |
| Raw `char*` for argv arrays            | ⚠️ Could use span |

### RAII

**Partial implementation:**

```cpp
// p5.cpp - RAII for P4API lifecycle
P5::P5() {
    if (!InitializeLibraries()) { return; }
    if (!Initialize()) { return; }
}

P5::~P5() {
    if (!Deinitialize()) { /* log error */ }
    if (m_LibrariesInitialized && !ShutdownLibraries()) { /* log error */ }
}
```

**Issue:** Constructor doesn't throw on failure, leaving object in invalid state.

**Recommendation:**

```cpp
class P5 {
public:
    P5(const Options& options) {
        if (!InitializeLibraries()) {
            throw P5InitializationError("Could not initialize P4 libraries");
        }
        // ...
    }

    ~P5() noexcept {  // noexcept for RAII destructor
        try {
            Deinitialize();
            ShutdownLibraries();
        } catch (...) {
            // Log but don't throw from destructor
        }
    }
};
```

### Move Semantics

**Underutilized:**

```cpp
// p5.cpp - string concatenation (inefficient)
template <class T>
T P5::Run(const char *command, const std::vector<std::string> &stringArguments, const int commandRetries) {
    std::string argsString;
    for (const std::string &stringArg : stringArguments) {
        argsString = argsString + " " + stringArg;  // ❌ Copy on each iteration
    }
    // ...
}
```

**Recommendation:**

```cpp
std::string argsString;
for (const auto& arg : stringArguments) {
    argsString += ' ';
    argsString += arg;  // Or use std::format in C++20
}
// Or even better with C++20:
// std::format("{}", fmt::join(stringArguments, " "));
```

### std::optional and std::variant

**Underutilized:**

```cpp
// Current: Returns empty string for "not found"
static std::string AutoResolve(const std::string &port, const std::string &user);
```

**Recommendation:**

```cpp
// Better: Explicit about possible failure
static std::optional<std::string> AutoResolve(const std::string &port, const std::string &user);
```

### C++17 Features Not Used

| Feature             | Opportunity                         |
| ------------------- | ----------------------------------- |
| `std::string_view`  | Pass string parameters without copy |
| `std::optional`     | Return values that may not exist    |
| `std::variant`      | Type-safe unions for result types   |
| `if constexpr`      | Compile-time branching              |
| `[[nodiscard]]`     | Prevent ignoring return values      |
| Structured bindings | Already used in some places ✅      |

---

## 6. Code Smells & Issues

### 1. Global State Misuse

**Location:** `options.h`

```cpp
extern Options g_options;  // Global mutable state
```

**Problems:**

- Hidden dependencies
- Thread-unsafe
- Testing nightmare
- Violates single responsibility

**Impact:** High - affects entire codebase testability.

### 2. Confusing Method Name

**Location:** `result.h`

```cpp
bool IsError() { return m_Error.IsError() == 0; }
```

**Problem:** Logic is inverted - returns `true` when there is NO error.

**Expected behavior:**

```cpp
if (result.IsError()) {
    // Handle error - but this actually means NO error!
}
```

**Current workaround in code:**

```cpp
if (r.IsError()) {  // Actually means "is success"
    r.PrintSortedTsv(std::cout);
}
```

### 3. God Object: main.cpp

**Location:** `main.cpp` (177 lines)

**Problem:** Contains registration of all 80+ commands, violating Open/Closed principle.

```cpp
// Every new command requires modifying main.cpp
commands.add("add", "...");
commands.add("delete", "...");
// ... 80+ lines of similar code
```

### 4. Duplicate Code

**Location:** `clients.cpp` and `users.cpp`

```cpp
// clients.cpp
void Clients::run(const std::vector<std::string> &args) {
    bool printDone = false;
    {
        P5 p5;
        Clients r = p5.ListClients(args);
        if (r.IsError()) {
            // print logic
            printDone = true;
        }
    }
    if (!printDone) {
        throw CLI::RuntimeError(1);
    }
}

// users.cpp - Nearly identical structure
void Users::run(const std::vector<std::string> &args) {
    bool printDone = false;
    {
        P5 p5;
        Users r = p5.ListUsers(args);
        if (r.IsError()) {
            r.PrintSortedTsv(std::cout);
            printDone = true;
        }
    }
    if (!printDone) {
        throw CLI::RuntimeError(1);
    }
}
```

### 5. Primitive Obsession

**Location:** Various

```cpp
// Using strings for everything
using ClientName = std::string;
using UserID = std::string;
```

**Recommendation:** Strong types for domain concepts.

```cpp
struct ClientName {
    std::string value;
    explicit ClientName(std::string v) : value(std::move(v)) {}
    // Validation in constructor
};
```

### 6. Long Function: p4_set_persistent_unix

**Location:** `set.cpp` (lines 158-253, 95 lines)

**Problems:**

- File I/O, directory creation, and config parsing in one function
- Hard to test individual components
- Multiple responsibilities

### 7. Test File Bloat

**Location:** `tests/test_log.cpp`

The file is **1002 lines**, with lines 99-999 being empty whitespace padding to test line number formatting. This is a maintainability issue.

---

## 7. Refactoring Recommendations

### Quick Wins (Low Effort, High Impact)

#### 1. Fix `IsError()` Method Name (15 min)

**Before:**

```cpp
bool IsError() { return m_Error.IsError() == 0; }
```

**After:**

```cpp
bool Success() const { return m_Error.IsError() == 0; }
bool Failed() const { return m_Error.IsError() != 0; }
```

**Benefit:** Eliminates confusing inverted logic throughout codebase.

#### 2. Add `[[nodiscard]]` Attributes (30 min)

```cpp
[[nodiscard]] bool Success() const;
[[nodiscard]] const ClientMap& GetClients() const;
[[nodiscard]] static std::optional<std::string> AutoResolve(...);
```

**Benefit:** Compiler enforces result usage, prevents bugs.

#### 3. Use `std::string_view` for String Parameters (1 hour)

**Before:**

```cpp
static bool EndsWith(const std::string &str, const std::string &checkStr);
```

**After:**

```cpp
static bool EndsWith(std::string_view str, std::string_view checkStr);
```

**Benefit:** Eliminates unnecessary string copies.

#### 4. Replace STDHelpers with C++20 std::string Functions (1 hour)

```cpp
// Current
bool STDHelpers::StartsWith(const std::string &str, const std::string &checkStr);

// C++20 replacement (no wrapper needed)
str.starts_with(checkStr);
str.ends_with(checkStr);
str.contains(subStr);
```

### Medium Refactors

#### 1. Extract Command Execution Pattern (4 hours)

**Problem:** Duplicate P5 instantiation and error handling.

**Solution:** Create a helper method.

```cpp
// Proposed helper in Command base class
template<typename ResultType>
void ExecuteCommand(
    std::function<ResultType(P5&)> executor,
    std::function<void(const ResultType&)> formatter
) {
    P5 p5;
    ResultType result = executor(p5);
    if (result.Success()) {
        formatter(result);
    } else {
        throw CLI::RuntimeError(1);
    }
}

// Usage in Clients::run
void Clients::run(const std::vector<std::string> &args) {
    ExecuteCommand<Clients>(
        [&](P5& p5) { return p5.ListClients(args); },
        [](const Clients& r) { r.PrintFormatted(std::cout); }
    );
}
```

**Benefit:** Eliminates duplicate code, consistent error handling.

#### 2. Dependency Injection for Options (4 hours)

**Before:**

```cpp
// Global access
extern Options g_options;

P5::P5() {
    m_ClientAPI.SetPort(g_options.port().c_str());
}
```

**After:**

```cpp
class P5 {
public:
    explicit P5(const Options& options);
    // ...
};

// In main.cpp or command
Options options;
P5 p5(options);
```

**Benefit:** Testable, explicit dependencies.

#### 3. Extract Output Formatting (6 hours)

```cpp
// Proposed: formatters.h
namespace formatters {

void PrintClientsTable(const Clients::ClientMap& clients, std::ostream& out);
void PrintUsersTable(const Users::UserMap& users, std::ostream& out);

// Future: JSON, XML, etc.
void PrintClientsJson(const Clients::ClientMap& clients, std::ostream& out);

}  // namespace formatters
```

**Benefit:** Separation of concerns, extensible output formats.

### Larger Architectural Changes

#### 1. Introduce IP4Client Interface (2 days)

```cpp
// include/p5/ip4_client.h
class IP4Client {
public:
    virtual ~IP4Client() = default;

    virtual void SetProtocol(std::string_view key, std::string_view value) = 0;
    virtual Result Run(std::string_view command, std::span<const std::string> args) = 0;
    virtual Clients ListClients(std::span<const std::string> args) = 0;
    virtual Users ListUsers(std::span<const std::string> args) = 0;
};

// Production implementation
class P4Client : public IP4Client { /* uses P4API */ };

// Test implementation
class MockP4Client : public IP4Client { /* returns fixtures */ };
```

**Benefit:** Full unit testability without P4 server.

#### 2. Command Auto-Registration (2 days)

```cpp
// include/p5/command_registry.h
class CommandRegistry {
public:
    static CommandRegistry& instance();

    void register_command(std::unique_ptr<Command> cmd);
    void install_all(CLI::App& app);

private:
    std::vector<std::unique_ptr<Command>> m_commands;
};

// Auto-registration macro
#define P5_REGISTER_COMMAND(CommandClass) \
    namespace { \
        struct CommandClass##Registrar { \
            CommandClass##Registrar() { \
                CommandRegistry::instance().register_command( \
                    std::make_unique<CommandClass>()); \
            } \
        } registrar_##CommandClass; \
    }

// In commands/clients.cpp
P5_REGISTER_COMMAND(Clients);

// main.cpp becomes
int main(int argc, char **argv) {
    CLI::App app(P5_APP_DESCRIPTION);
    g_options.add(app);
    CommandRegistry::instance().install_all(app);
    app.parse(argc, argv);
}
```

**Benefit:** Open/Closed principle - add commands without touching main.cpp.

#### 3. Comprehensive Error Handling Strategy (3 days)

**Current state:** Mixed use of `ERROR` macro, `CLI::RuntimeError`, and silent failures.

**Proposed:** Unified error handling.

```cpp
// include/p5/error.h
enum class ErrorCode {
    Success = 0,
    ConnectionFailed = 1,
    AuthenticationFailed = 2,
    CommandFailed = 3,
    InvalidArguments = 4,
    ConfigError = 5,
};

class P5Error : public std::runtime_error {
public:
    explicit P5Error(ErrorCode code, std::string message)
        : std::runtime_error(std::move(message)), m_code(code) {}

    ErrorCode code() const noexcept { return m_code; }

private:
    ErrorCode m_code;
};

// Usage
void Clients::run(const std::vector<std::string>& args) {
    try {
        P5 p5(options_);
        auto result = p5.ListClients(args);
        if (result.Failed()) {
            throw P5Error(ErrorCode::CommandFailed, "Failed to list clients");
        }
        formatters::PrintClientsTable(result.GetClients(), std::cout);
    } catch (const P5Error& e) {
        CLI_ERROR(e.what());
        throw CLI::RuntimeError(static_cast<int>(e.code()));
    }
}
```

---

## 8. Scalability & Maintainability

### Current Scalability Issues

| Issue                        | Impact                                                 | Severity |
| ---------------------------- | ------------------------------------------------------ | -------- |
| All commands in main.cpp     | Every new command requires main.cpp change             | High     |
| No output format abstraction | Adding JSON/XML output requires touching every command | High     |
| Tight P4API coupling         | Cannot swap or mock P4API                              | Medium   |
| Global options               | Thread safety issues, testing problems                 | Medium   |

### Future Development Bottlenecks

1. **Adding new output formats** (e.g., JSON for scripting)
   - Current: Must modify every command class
   - Solution: Strategy pattern for output formatting

2. **Parallel command execution** (for bulk operations)
   - Current: Global state prevents parallelization
   - Solution: Remove global state, make P5 thread-safe

3. **Plugin system** (custom commands)
   - Current: All commands compiled in
   - Solution: Dynamic command loading via interfaces

### Maintainability Recommendations

1. **Add comprehensive documentation**
   - Document architecture decisions
   - API documentation with Doxygen
   - Contribution guidelines

2. **Establish coding standards**
   - `.clang-format` exists ✅
   - Add `.clang-tidy` for static analysis rules
   - Document naming conventions

3. **Improve error messages**
   - Current: Mix of log levels and formats
   - Proposed: Structured error types with context

---

## 9. Testing & Tooling

### Current Testability Assessment

| Component      | Testable?  | Reason                       |
| -------------- | ---------- | ---------------------------- |
| Log            | ✅ Yes     | Standalone, no dependencies  |
| STDHelpers     | ✅ Yes     | Pure functions               |
| ClientResolver | ⚠️ Partial | Depends on P4API's HostEnv   |
| P5             | ❌ No      | Requires real P4 server      |
| Commands       | ❌ No      | Create P5 instances directly |
| Options        | ⚠️ Partial | Global state issues          |

### Testing Infrastructure Recommendations

#### 1. Add Test Framework

```cmake
# CMakeLists.txt
option(BUILD_TESTING "Build tests" ON)

if(BUILD_TESTING)
    include(FetchContent)
    FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.5.0
    )
    FetchContent_MakeAvailable(Catch2)

    enable_testing()
    add_subdirectory(tests)
endif()
```

#### 2. Create Test Utilities

```cpp
// tests/test_helpers.h
class MockOptions {
public:
    MockOptions& set_port(std::string_view port) { port_ = port; return *this; }
    MockOptions& set_user(std::string_view user) { user_ = user; return *this; }
    // ...
};

class MockP4Client : public IP4Client {
public:
    MOCK_METHOD(void, SetProtocol, (std::string_view, std::string_view), (override));
    MOCK_METHOD(Result, Run, (std::string_view, std::span<const std::string>), (override));
    // ...
};
```

#### 3. Example Unit Tests

```cpp
// tests/test_client_resolver.cpp
#include <catch2/catch_test_macros.hpp>
#include "utils/client_resolver.h"

TEST_CASE("ClientResolver::Resolve returns longest matching root") {
    Clients::ClientMap clients;
    clients["client_a"] = {.root = "/home/user/project"};
    clients["client_b"] = {.root = "/home/user/project/subdir"};

    REQUIRE(ClientResolver::Resolve("/home/user/project/subdir/src", clients)
            == "client_b");
    REQUIRE(ClientResolver::Resolve("/home/user/project/other", clients)
            == "client_a");
}

TEST_CASE("ClientResolver::Resolve returns empty for no match") {
    Clients::ClientMap clients;
    clients["client_a"] = {.root = "/home/user/project"};

    REQUIRE(ClientResolver::Resolve("/other/path", clients).empty());
}
```

### Recommended Tooling

| Tool                     | Purpose                    | Configuration                  |
| ------------------------ | -------------------------- | ------------------------------ |
| **clang-tidy**           | Static analysis            | `.clang-tidy` file             |
| **clang-format**         | Code formatting            | Already configured ✅          |
| **cppcheck**             | Additional static analysis | CI integration                 |
| **sanitizers**           | Memory safety              | `-fsanitize=address,undefined` |
| **include-what-you-use** | Include cleanup            | CMake integration              |

#### clang-tidy Configuration

```yaml
# .clang-tidy
Checks: >
  -*,
  bugprone-*,
  cppcoreguidelines-*,
  modernize-*,
  performance-*,
  readability-*,
  -modernize-use-trailing-return-type,
  -readability-magic-numbers
WarningsAsErrors: ""
HeaderFilterRegex: ".*"
CheckOptions:
  - key: readability-identifier-naming.MemberCase
    value: camelBack
  - key: readability-identifier-naming.MemberPrefix
    value: "m_"
```

### CI/CD Recommendations

```yaml
# .github/workflows/ci.yml
name: CI

on: [push, pull_request]

jobs:
  build-and-test:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4
        with:
          submodules: recursive

      - name: Install dependencies
        run: |
          sudo apt-get update
          sudo apt-get install -y cmake ninja-build libssl-dev libzstd-dev

      - name: Configure
        run: cmake --preset default

      - name: Build
        run: cmake --build --preset default

      - name: Run tests
        run: ctest --preset default

      - name: Static analysis
        run: |
          clang-tidy source/**/*.cpp -- -Iinclude/p5 -Ivendor/CLI11/include

  format-check:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4
      - name: Check formatting
        run: ./scripts/format_cpp.sh --dry-run
```

---

## Summary

### Priority Matrix

| Refactoring                       | Effort  | Impact    | Priority |
| --------------------------------- | ------- | --------- | -------- |
| Fix `IsError()` naming            | 15 min  | High      | 🔴 P0    |
| Add `[[nodiscard]]`               | 30 min  | Medium    | 🟡 P1    |
| Use `std::string_view`            | 1 hour  | Medium    | 🟡 P1    |
| Extract command execution pattern | 4 hours | High      | 🟠 P2    |
| Dependency injection for Options  | 4 hours | High      | 🟠 P2    |
| Extract output formatting         | 6 hours | Medium    | 🟡 P2    |
| Introduce IP4Client interface     | 2 days  | Very High | 🔴 P3    |
| Command auto-registration         | 2 days  | High      | 🟠 P3    |
| Error handling strategy           | 3 days  | High      | 🟠 P3    |

### Key Takeaways

1. **Architecture is solid** - The layered approach with CLI11 and P4API abstraction is appropriate.

2. **Testability is the biggest gap** - The global `g_options` and tight P4API coupling make unit testing nearly impossible.

3. **Code quality is generally good** - Naming is consistent, structure is logical, modern C++ is used (though not fully).

4. **Quick wins available** - Several improvements can be made in hours with significant impact.

5. **Strategic investments needed** - The IP4Client interface and command auto-registration require more effort but enable long-term scalability.

### Recommended Roadmap

**Phase 1 (Week 1): Quick Wins**

- Fix `IsError()` naming
- Add `[[nodiscard]]` attributes
- Start using `std::string_view` and `std::optional`

**Phase 2 (Month 1): Testability**

- Introduce IP4Client interface
- Add dependency injection for Options
- Set up Catch2 testing framework

**Phase 3 (Month 2-3): Scalability**

- Implement command auto-registration
- Extract output formatting with Strategy pattern
- Add comprehensive error handling

**Phase 4 (Ongoing): Quality**

- Expand test coverage
- Add static analysis to CI
- Document architecture decisions
