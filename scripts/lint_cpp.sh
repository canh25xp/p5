#!/usr/bin/env bash
# Lint all git-tracked C/C++ source files with clang-tidy.
# Requires a compile_commands.json
#
# Usage:
#   ./scripts/lint_cpp.sh                        # lint, print warnings
#   ./scripts/lint_cpp.sh --fix                  # apply safe auto-fixes
#   ./scripts/lint_cpp.sh --fix-errors           # apply all auto-fixes (incl. risky ones)
#   FIX=1 ./scripts/lint_cpp.sh                  # same as --fix via env var
#   CLANG_TIDY=clang-tidy-18 ./scripts/lint_cpp.sh
#   BUILD_DIR=build/release ./scripts/lint_cpp.sh

set -euo pipefail

cd "$(git rev-parse --show-toplevel)"

# ---------------------------------------------------------------------------
# Config (override via env)
# ---------------------------------------------------------------------------
clang_tidy="${CLANG_TIDY:-clang-tidy}"
build_dir="${BUILD_DIR:-build}"
jobs="${JOBS:-$(nproc 2>/dev/null || sysctl -n hw.logicalcpu 2>/dev/null || echo 4)}"

fix_mode=""
for arg in "$@"; do
    case "$arg" in
    --fix) fix_mode="--fix" ;;
    --fix-errors) fix_mode="--fix --fix-errors" ;;
    *)
        echo "Unknown argument: $arg" >&2
        exit 1
        ;;
    esac
done
[[ "${FIX:-0}" == "1" ]] && fix_mode="--fix"

# ---------------------------------------------------------------------------
# Verify clang-tidy
# ---------------------------------------------------------------------------
if ! command -v "$clang_tidy" >/dev/null 2>&1; then
    echo "error: $clang_tidy not found." >&2
    echo "  Install: sudo apt install clang-tidy  |  brew install llvm" >&2
    echo "  Or set CLANG_TIDY=/path/to/clang-tidy" >&2
    exit 1
fi

version=$("$clang_tidy" --version | head -1)
echo "Using: $version"

# ---------------------------------------------------------------------------
# Locate compile_commands.json
# ---------------------------------------------------------------------------
compile_commands=""
search_dirs=("$build_dir" "build" ".")
for d in "${search_dirs[@]}"; do
    candidate="$d/compile_commands.json"
    if [[ -f "$candidate" ]]; then
        compile_commands="$(realpath "$candidate")"
        break
    fi
done

p_flag=()
if [[ -n "$compile_commands" ]]; then
    echo "Compile commands: $compile_commands"
    p_flag=("-p" "$compile_commands")
else
    echo "warning: compile_commands.json not found; clang-tidy will use best-effort defaults." >&2
    echo "  Tip: cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -B $build_dir" >&2
fi

# ---------------------------------------------------------------------------
# Collect files (source files only; headers are pulled in transitively)
# ---------------------------------------------------------------------------
files=()
while IFS= read -r -d '' f; do
    files+=("$f")
done < <(git ls-files -z -- \
    '*.cpp' '*.cc' '*.cxx' '*.c' \
    ':(exclude)vendor/' \
    ':(exclude)third_party/' \
    ':(exclude)tests/')

if ((${#files[@]} == 0)); then
    echo "No tracked C/C++ source files matched."
    exit 0
fi

echo "Files to lint: ${#files[@]}"
[[ -n "$fix_mode" ]] && echo "Fix mode: $fix_mode"
echo "---"

# ---------------------------------------------------------------------------
# Run clang-tidy (parallel with xargs, or sequential fallback)
# ---------------------------------------------------------------------------
exit_code=0

run_one() {
    local f="$1"
    # shellcheck disable=SC2086
    "$clang_tidy" "${p_flag[@]}" $fix_mode "$f" 2>&1
}
export -f run_one
export clang_tidy fix_mode
export p_flag_str="${p_flag[*]:-}" # xargs can't export arrays; reassemble inside

# Rebuild p_flag inside child processes via a wrapper script in a temp file
wrapper=$(mktemp /tmp/lint_wrapper_XXXXXX.sh)
trap 'rm -f "$wrapper"' EXIT

cat >"$wrapper" <<'EOF'
#!/usr/bin/env bash
# Reconstructed inside xargs worker
clang_tidy="${CLANG_TIDY:-clang-tidy}"
p_flag=()
[[ -n "${p_flag_str:-}" ]] && read -ra p_flag <<< "$p_flag_str"
fix_mode="${fix_mode:-}"
file="$1"
# shellcheck disable=SC2086
exec "$clang_tidy" "${p_flag[@]}" $fix_mode "$file"
EOF
chmod +x "$wrapper"

if command -v xargs >/dev/null 2>&1 && xargs --version 2>&1 | grep -q GNU; then
    # GNU xargs supports -P for parallelism
    printf '%s\0' "${files[@]}" |
        xargs -0 -P "$jobs" -I{} bash "$wrapper" {} ||
        exit_code=$?
else
    # BSD / macOS xargs or fallback: sequential
    for f in "${files[@]}"; do
        bash "$wrapper" "$f" || exit_code=$?
    done
fi

echo "---"
if ((exit_code == 0)); then
    printf 'Lint passed: %d file(s) checked.\n' "${#files[@]}"
else
    printf 'Lint FAILED on one or more of %d file(s).\n' "${#files[@]}" >&2
    exit "$exit_code"
fi
