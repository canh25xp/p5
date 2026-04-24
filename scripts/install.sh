#!/usr/bin/env bash
set -euo pipefail

cd "$(git rev-parse --show-toplevel)"

usage() {
    cat <<'USAGE'
Usage: install.sh [-h|--help]

Copies the release build (default: build/Release/p5) into ~/.local/bin (default). Set
`P5_BINARY` or `DEST_DIR` to override either path.
USAGE
}

while [[ $# -gt 0 ]]; do
    case "$1" in
        -h|--help)
            usage
            exit 0
            ;;
        *)
            echo "Unsupported argument: $1" >&2
            usage
            exit 1
            ;;
    esac
    shift
done

P5_BINARY="${P5_BINARY:-build/Release/p5}"
DEST_DIR="${DEST_DIR:-$HOME/.local/bin}"

if [[ ! -f "$P5_BINARY" ]]; then
    echo "error: source binary not found at '$P5_BINARY'" >&2
    exit 1
fi

if [[ ! -x "$P5_BINARY" ]]; then
    echo "error: source binary at '$P5_BINARY' is not executable" >&2
    exit 1
fi

mkdir -p "$DEST_DIR"
install -Dm755 "$P5_BINARY" "$DEST_DIR/$(basename "$P5_BINARY")"

echo "Installed '$P5_BINARY' to '$DEST_DIR/$(basename "$P5_BINARY")'"
