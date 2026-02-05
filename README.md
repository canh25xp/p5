# p5

> Complementary p4 command-line tools

> [!WARNING]
> WIP: Barely even started.
> Start this project thinking "How hard can it be".
> Turns out it is darn hard.

## Dependencies

> [!NOTE]
> cmake will download p4api for you, so no need for manual download, it just here for references

You can download p4api from the [official website](https://www.perforce.com/downloads/helix-core-c/c-api) or the [archive](https://filehost.perforce.com/perforce/r25.1/bin.mingw64x64/p4api-openssl3_gcc8_win32_seh.zip)

```sh
curl -Lo p4api.zip https://filehost.perforce.com/perforce/r25.1/bin.mingw64x64/p4api-openssl3_gcc8_win32_seh.zip
```

### Linux

```sh
sudo apt install libssl-dev
```

### Windows

First open mingw64 shell: `C:/msys64/msys2_shell.cmd -defterm -here -no-start -mingw64`

```sh
pacman -S mingw-w64-x86_64-toolchain
pacman -S mingw-w64-x86_64-openssl
```

Add `C:/msys64/mingw64/bin` to your PATH environment variable

## Build

Clone repo and submodules

```sh
git clone https://github.com/canh25xp/p5.git
git submodule update --init
```

Build it:

```sh
cmake -B build -G "MinGW Makefiles"
cmake --build build
# or
cmake --workflow default
```
