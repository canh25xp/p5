# Auto resolve client

## Problem

Current problem with p4: user may own multiple clients in a single host machine:

```console
$ p4 clients --me
Client SRV_DSS_SECSETTINGS_BENI_VANCANH_NG 2026/03/13 root /home/vancanh-ng/projects/SecSettings_BENI 'Created by vancanh.ng. '
Client SRV_DSS_SECSETTINGS_COOSA_VANCANH_NG 2026/04/08 root /home/vancanh-ng/projects/SecSettings_COOSA 'Created by vancanh.ng. '
Client swarm-6cf5aa8b-e057-0c87-4b96-35f7b798d70a 2025/04/08 root /opt/perforce/swarm/data/clients/76616E63616E682E6E67/swarm-6cf5aa8b-e057-0c87-4b96-35f7b798d70a 'Created by vancanh.ng. '

$ p4 -z tag clients --me
... client SRV_DSS_SECSETTINGS_BENI_VANCANH_NG
... Update 1773382989
... Access 1774861868
... Owner vancanh.ng
... Options allwrite noclobber nocompress unlocked nomodtime normdir noaltsync
... SubmitOptions leaveunchanged
... LineEnd local
... Root /home/vancanh-ng/projects/SecSettings_BENI
... AltRoots0 C:\Users\vancanh.ng\projects\SRV_DSS_SECSETTINGS_BENI_VANCANH_NG
... Host vancanh-ng02
... Type writeable
... Backup enable
... Description Created by vancanh.ng.


... client SRV_DSS_SECSETTINGS_COOSA_VANCANH_NG
... Update 1775629665
... Access 1776761264
... Owner vancanh.ng
... Options allwrite noclobber nocompress unlocked nomodtime normdir noaltsync
... SubmitOptions leaveunchanged
... LineEnd local
... Root /home/vancanh-ng/projects/SecSettings_COOSA
... AltRoots0 C:/Users/vancanh.ng/projects/SRV_DSS_SECSETTINGS_COOSA_VANCANH_NG
... Host vancanh-ng02
... Type writeable
... Backup enable
... Description Created by vancanh.ng.


... client swarm-6cf5aa8b-e057-0c87-4b96-35f7b798d70a
... Update 1744097757
... Access 1747738146
... Owner vancanh.ng
... Options noallwrite noclobber nocompress unlocked nomodtime normdir noaltsync
... SubmitOptions leaveunchanged
... LineEnd local
... Root /opt/perforce/swarm/data/clients/76616E63616E682E6E67/swarm-6cf5aa8b-e057-0c87-4b96-35f7b798d70a
... Host
... Type writeable
... Backup enable
... Description Created by vancanh.ng.
```

Each time user want to switch Client, they must manually specify `P4CLIENT`.

## Solution

Since each client **usually** only corresponding to an unique `root` on the host machine.
We can first **guess** the client based on cwd (current working directory) and then set P4CLIENT appropriately.

## Implementation

1. First implement `GetClients` method that returns all clients own by current user (`--me`) flag
   and on current `Host` (Current API does not support filter Host, need implement, maybe a `--here` or `--this` flag to indicate current machine ?)
2. Optionally cache this client list somewhere
3. Implement `GetCwd` (p4api might already implement this)
4. Implement resolve client based on cwd.
5. Set P4CLIENT based on resolved client.

Note:
- If P4CLIENT environment is set or `-c --client` is provided. Then use it instead.
- Optionally specify to enabled to disable this function on compile time or run time.
