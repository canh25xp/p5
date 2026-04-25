# Auto resolve client

## Problem

Current problem with p4: user may own multiple clients in a single host machine:

```console
$ p4 clients --me
Client SRV_DSS_SECSETTINGS_BENI_VANCANH_NG 2026/03/13 root /home/vancanh-ng/projects/SecSettings_BENI 'Created by vancanh.ng. '
Client SRV_DSS_SECSETTINGS_COOSA_VANCANH_NG 2026/04/08 root /home/vancanh-ng/projects/SecSettings_COOSA 'Created by vancanh.ng. '
Client swarm-6cf5aa8b-e057-0c87-4b96-35f7b798d70a 2025/04/08 root /opt/perforce/swarm/data/clients/76616E63616E682E6E67/swarm-6cf5aa8b-e057-0c87-4b96-35f7b798d70a 'Created by vancanh.ng. '
```

Each time user want to switch Client, they must manually specify `P4CLIENT`.

## Solution

Since each client **usually** only corresponds to a unique `root` on the host machine,
we can first **guess** the client based on cwd (current working directory)
and then set P4CLIENT appropriately.

## Implementation

### What was implemented

1. **`Clients` command** (`include/p5/commands/clients.h`, `source/commands/clients.cpp`)
   - Parses `p4 clients` tagged output (client name, Root, Host, AltRoots, Description)
   - `--me` flag: lists clients owned by the current user
   - `--here` flag: filters results to clients on the current host (implies `--me`)
   - Formatted multi-line output (one block per client)
   - Replaces the passthrough `clients`/`workspaces` subcommand

2. **`ListClients` method on P5** (`include/p5/p5.h`, `source/p5.cpp`)
   - Runs `p4 clients` with tag protocol and returns a `Clients` result object
   - Used by both the `p5 clients` command and the auto-resolve feature

3. **`ClientResolver` utility** (`include/p5/utils/client_resolver.h`, `source/utils/client_resolver.cpp`)
   - `Resolve(cwd, clients)`: finds the client whose Root (or AltRoots) is the longest
     path prefix of the current working directory
   - `GetCurrentWorkingDirectory()`: gets CWD via p4api's `HostEnv::GetCwd`
   - `GetCurrentHostname()`: gets hostname via p4api's `HostEnv::GetHost`
   - `FilterByHost(clients, host)`: filters clients to those matching the given host;
     clients with empty host field (e.g., Swarm clients) are included

4. **Auto-resolve integration** (`source/p5.cpp`)
   - `P5::AutoResolveClient()` is called from `P5::Initialize()` when no explicit
     client is set
   - Fetches clients with `--me`, filters by current host, resolves by CWD
   - Sets the resolved client via `m_ClientAPI.SetClient()`
   - Re-entry guard (`m_AutoResolving`) prevents infinite recursion if
     `ListClients` triggers a `Reinitialize`

5. **`--resolve-client` global flag** (`include/p5/options.h`, `source/options.cpp`)
   - Controls automatic client resolution at runtime (defaults to `true`)
   - Use `--no-resolve-client` to disable
   - Priority: explicit `-c`/`--client`/`P4CLIENT` env > auto-resolve > default

### Not yet implemented

- Client list caching (optional optimization to avoid `p4 clients --me` on every command)
- Compile-time toggle to disable auto-resolve
- Skip resolve clients for commands that does not access P4CLIENT, for example `p4 changes`, `p4 users`, ...
