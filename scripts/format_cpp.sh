#!/usr/bin/env bash
# Format all git-tracked C/C++ source files with clang-format using the repo's .clang-format.
# Usage: ./scripts/format_cpp.sh
#        bash scripts/format_cpp.sh

set -euo pipefail

cd "$(git rev-parse --show-toplevel)"

clang_format="${CLANG_FORMAT:-clang-format}"
if ! command -v "$clang_format" >/dev/null 2>&1; then
    echo "error: $clang_format not found (set CLANG_FORMAT to a specific binary if needed)" >&2
    exit 1
fi

files=()
while IFS= read -r -d '' f; do
    files+=("$f")
done < <(git ls-files -z -- '*.cpp' '*.cc' '*.cxx' '*.h' '*.hpp' '*.hh' '*.hxx' ':(exclude)vendor/')

if ((${#files[@]} == 0)); then
    echo "No tracked C++ files matched."
    exit 0
fi

"$clang_format" -i -style=file -- "${files[@]}"
printf 'Formatted %d file(s).\n' "${#files[@]}"
