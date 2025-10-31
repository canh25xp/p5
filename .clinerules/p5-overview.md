# P5 Development Guide

## Project Overview

`p5` is a command line tools that meant to replace the `p4` command line tool.
It's add features that `p4` does not have and improve features that `p4` already has, both visually and functionally.

## Project Guideline

The command line interface should be identical or at least close to the existing `p4` tool to help user easily migrate to `p5`.

## Development Guide

### Build and test

```sh
cmake --workflow default # both generate and build in a single workflow
# or
cmake --preset default && cmake --build --preset default
# or
cmake -B build && cmake --build build
```

Test it

```sh
p5 --help
# or
./build/p5 --help
```

### Command line guide

This project uses `CLI11` library for command line handling.
The documents is at `vendor/CLI11/book/`
