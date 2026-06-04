# p5 mirror

- Need a command to behave like `git clone`
- Current `p4 clone` name has taken (clone -- Clone a new personal server from a shared server).
  However the behaviour of `p4 clone` is not quite what I want.
  I need to clone a specific client, not whole server.
- Implement `p4 mirror` which taken 1 positional argument which is client name.
  Then create a client for current user with exact mapping, synced file as the mirror client.

## Problem

The process of create a client with exact files and revision is troublesome.

1. First create a new client with template client name.
   ```sh
   p4 client -t TEMPLATE_CLIENT MIRRORED_CLIENT
   ```
2. CD to the created client and point P4CLIENT to that
   ```sh
   cd MIRRORED_CLIENT
   cat `P4CLIENT=MIRRORED_CLIENT` > .p4config
   ```
3. Get latest synced CL of the template client
   ```sh
   p4 -c TEMPLATE_CLIENT changes -m1 //TEMPLATE_CLIENT/...#have
   Change 123456 on 2026/06/01 by user.id@TEMPLATE_CLIENT 'desciption'
   ```
4. Sync the mirror client:
   ```sh
   p4 sync ...@123456
   ```

And this is to assuming the TEMPLATE_CLIENT did NOT sync any partial CLs.
In which case you might have to get all synced CLs of the template client:

```sh
p4 -c TEMPLATE_CLIENT changes //TEMPLATE_CLIENT/...#have
Change 123456 on 2026/06/01 by user.id@TEMPLATE_CLIENT 'desciption'
...
Change 100000 on 2026/01/01 by user.id@TEMPLATE_CLIENT 'desciption'
```

And then sync each CLs manually:

```sh
p4 sync ...@100000
...
p4 sync ...@123456
```

## Solution

- A `p5 mirror` command which do all of the above

```sh
# Default name to TEMPLATE_CLIENT + user name prefix, for example JANE_DOE_TEMPLATE_CLIENT
p5 mirror TEMPLATE_CLIENT
p5 mirror TEMPLATE_CLIENT MIRRORED_CLIENT # optionally specify mirror client name
```
