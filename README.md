# p5

> Complementary p4 command-line tools

> [!WARNING]
> WIP: Barely even started.
> Start this project thinking "How hard can it be".
> Turns out it is darn hard.

## Dependencies

### P4 api

[cmake](./cmake/downloadP4api.cmake) will automatically download p4api from the [official website](https://www.perforce.com/downloads/helix-core-c/c-api) or the [archive](https://filehost.perforce.com/perforce/r25.2)

### Linux

```sh
sudo apt install libssl-dev
sudo apt install libzstd-dev
```

### Windows

**Requirements (any Windows build)**

- 64-bit Windows 10 or later
- Git (clone and `git submodule`)
- CMake 3.15 or newer (3.18+ recommended: archive extract uses CMake’s `file(ARCHIVE_EXTRACT)` when the Helix C++ API is downloaded)
- OpenSSL **3.x** with headers and import/static libraries (the Perforce API archives are built against OpenSSL 3)
- A C++ toolchain: **MSVC** (matches the `p4api_vs2022_static` / `p4api_vs2017_static` bundles CMake downloads) **or** **MinGW-w64** via MSYS2 (below)

**Install with winget (preferred)**

In an elevated PowerShell window if a package prompts for it:

```powershell
winget install -e --id Git.Git
winget install -e --id Kitware.CMake
```

**Visual Studio Build Tools** with the MSVC **C++ build tools** workload (`Microsoft.VisualStudio.Workload.VCTools`, includes a Windows SDK via recommended components). Prefer doing it from the shell in one step:

```powershell
winget install -e --id Microsoft.VisualStudio.2022.BuildTools --override "--wait --passive --add Microsoft.VisualStudio.Workload.VCTools --includeRecommended"
```

If Build Tools is already installed without that workload, add it with **Visual Studio Installer**’s CLI (same tool `winget` uses under the hood):

```powershell
$vswhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
$vsi     = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vs_installer.exe"
$installPath = & $vswhere -latest -products Microsoft.VisualStudio.Product.BuildTools -property installationPath
if (-not $installPath) { $installPath = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\2022\BuildTools" }
& $vsi modify --installPath $installPath --add Microsoft.VisualStudio.Workload.VCTools --includeRecommended --passive --wait --norestart
```

You can still use the **Visual Studio Installer** GUI (**Modify** → **Desktop development with C++** / **C++ build tools**) if you prefer.

For **OpenSSL**, the `msvc2022` / `msvc2017` CMake presets in this repo default to FireDaemon’s layout under `C:/Program Files/FireDaemon OpenSSL 3`. Installing that distribution avoids extra flags:

```powershell
winget install -e --id FireDaemon.OpenSSL
```

If you use another build (for example **Shining Light**), pass your install root when configuring (exact path depends on the package; look for `include/openssl` and `lib` next to each other):

```powershell
winget install -e --id ShiningLight.OpenSSL
# then, for example:
cmake --preset msvc2022 -D OPENSSL_ROOT_DIR="C:/Program Files/OpenSSL-Win64"
```

**Install with Scoop**

```powershell
scoop install git cmake openssl
scoop bucket add extras
scoop install vs2022buildtools
```

If the Scoop package only bootstraps the installer, add the same **C++ build tools** workload with the `vs_installer modify` PowerShell block above. If CMake cannot find OpenSSL, set `OPENSSL_ROOT_DIR` to the directory that contains `include/openssl` and `lib` (for Scoop, that is often the path printed by `scoop prefix openssl`).

**Build with MSVC (recommended)**

Use the **x64 Native Tools Command Prompt for VS 2022** (or any shell where `cl` and `cmake` are on `PATH`), then from the repo root:

```powershell
git submodule update --init
cmake --preset msvc2022
cmake --build --preset msvc2022-release
```

The executable is written under `build/Release/` (or `build/Debug/` with `msvc2022-debug`). Visual Studio 2017 is also supported via `--preset msvc2017` and the matching `msvc2017-*` build presets; the CMake step downloads the `vs2017_static` API archives when that generator is selected.

**Build with MinGW (MSYS2)**

Open a MinGW64 shell: `C:/msys64/msys2_shell.cmd -defterm -here -no-start -mingw64`

```sh
pacman -S mingw-w64-x86_64-toolchain
pacman -S mingw-w64-x86_64-openssl
```

Add `C:/msys64/mingw64/bin` to your `PATH`, then configure with **MinGW Makefiles** (see **Build** below), or use the `mingw-make-*` CMake presets, which assume OpenSSL under `C:/msys64/mingw64`.

## Build

Clone repo and submodules

```sh
git clone https://github.com/canh25xp/p5.git
git submodule update --init
```

Build it:

**Linux / default (Ninja in `CMakePresets.json`)**

```sh
cmake --workflow default
```

**Windows, MSVC (after installing the prerequisites above)**

```powershell
cmake --preset msvc2022
cmake --build --preset msvc2022-release
```

**Windows, MinGW**

```sh
cmake -B build -G "MinGW Makefiles"
cmake --build build
```

The first configure step downloads and extracts the Helix C++ API into `vendor/p4api/`; that requires network access and matches the selected toolchain (Visual Studio generator vs MinGW).
