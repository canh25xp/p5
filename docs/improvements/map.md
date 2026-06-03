# p5 map

Not part of `p4`. Edit the **View** field of the current (or `-c`) client workspace spec without opening an editor.

Uses `p4 client -o` / `p4 client -i` under the hood. Mapping lines are validated with the Helix `MapApi` before save. See `p4 help views` for view syntax (`-`, `+`, `&`, wildcards).

## Requirements

- `P4PORT`, `P4USER`, and `P4CLIENT` (or global `-c`) must be set.
- Stream clients (`Stream:` set in the spec) are rejected; the view is server-managed.

## Commands

| Command | Description |
|---------|-------------|
| `p5 map` | List View lines with 1-based indices (same as `list`) |
| `p5 map list` | List View lines |
| `p5 map add <mapping...>` | Append a mapping and save |
| `p5 map insert [--first\|--last\|-i N] <mapping...>` | Insert at a position (default: `--last`) |
| `p5 map edit -i N <mapping...>` | Replace line N |
| `p5 map delete [--first\|--last\|-i N]` | Remove a line |

## Mapping arguments

- Full mapping: `//depot/path/... //client/path/...` (quote paths that contain spaces).
- **Quick-add:** a single depot path `//depot/...` expands to  
  `//depot/... //{P4CLIENT}/depot/...` (depot name kept on the client side).
- Prefixes `-`, `+`, `&` on the depot path are preserved.

## Examples

```sh
p5 map
p5 map add //depot/project/...
p5 map insert --first "//depot/a/... //myclient/depot/a/..."
p5 map edit -i 2 "//depot/b/... //myclient/depot/b/..."
p5 map delete --last
p5 -c OTHER_WS map list
```
