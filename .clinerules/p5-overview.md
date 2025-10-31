# P5 Development Guide

## Project Overview

`p5` is a command line tools that meant to replace the `p4` command line tool.
It's add features that `p4` does not have and improve features that `p4` already has, both visually and functionally.

## Project Guideline

The command line interface should be identical or at least close to the existing `p4` tool to help user easily migrate to `p5`.

## Development setup

1. Clone the repository
   ```bash
   git clone https://github.com/canh25xp/p5.git
   ```
2. Init submodule:
   ```bash
   git submodule update --init --recursive
   ```
3. Install dependency
   ```bash
   sudo apt install libssl-dev
   ```
4. Build it with cmake
   ```bash
   cmake --workflow default
   ```
