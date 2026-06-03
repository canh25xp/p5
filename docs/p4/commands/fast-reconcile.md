# p5 fast-reconcile

Fast offline reconcile, based on the algorithm from [p4-fast-reconcile](https://github.com/brickadia/p4-fast-reconcile) (MIT).

## Usage

```sh
p5 fast-reconcile [-c change#] [-a -e -d -M -f -I -l -m -n -t] [--parallel=N] [file ...]
p5 fast-rec ...
```

Alias: `fast-rec`

## Behavior

Equivalent to `p4 reconcile` for default offline reconcile (add / edit / delete / revert unchanged), but faster on large workspaces:

1. Bulk `fstat` for depot state
2. Local workspace scan (with `P4IGNORE` unless `-I`)
3. Optional `-m` sync-time optimization via `p4 have`
4. Parallel MD5 digest checks when needed
5. Batched `add` / `edit` / `delete` / `revert` operations

## Supported flags

| Flag | Meaning |
|------|---------|
| *(none)* | Apply changes |
| `-n` | Preview only |
| `-c` | Target changelist |
| `-e` / `-a` / `-d` | Limit to edit / add / delete |
| `-m` | Skip digest when mtime ≤ sync time |
| `-I` | Skip P4IGNORE filtering |

## Passthrough

Unsupported reconcile flags (`-w`, `-k`, `-M`, `-t`, `-f`, `-l`, `--parallel`, etc.) are forwarded to `p4 reconcile` unchanged.

## Known limitations

- No `-M` move/add/delete pairing (same as p4-fast-reconcile)
- Apple / resource file types fall back to `p4 reconcile` for those paths

## See also

- [reconcile.md](reconcile.md) — full `p4 reconcile` documentation
- [fast-reconcise.md](../../improvements/fast-reconcise.md) — implementation notes (architecture, source layout, limitations)
