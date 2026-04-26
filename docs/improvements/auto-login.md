# Auto login

## Problem

Currently `p4 login` will ask for user password and generate a ticket that valid for 12 hours (Maybe defined by p4 server).
After ticket expired, users must re-login, .i.e re-enter password.
Doing that each day could be a little of a pain.
Especially in automated tasks where p4 may blocking the flow by wait for using login.

## Solution

Implement a **remember password** feature for `p5` to perform auto login each day.
Password should NOT store as clear text on machine and should be encrypted.

## Usage

```sh
p5 login --save
```

`p5 login --save` behaves like `p4 login`, captures the password entered at the normal password prompt, and stores it encrypted for the resolved `P4PORT` + `P4USER`.

After that, normal `p5` passthrough commands first check `p4 login -s`. If the current ticket is invalid and a saved password exists, `p5` runs `p4 login` with the saved password before running the requested command.

Authentication management commands (`login`, `login2`, `logout`, `tickets`, `trust`, and `help`) do not trigger auto login.

## Implementation

1. `AuthStore::SavePassword()` and `AuthStore::LoadPassword()` persist encrypted credentials under the user's local `p5` config directory.
2. `P5::AutoLogin()` checks ticket status with `login -s`, loads a saved password when needed, and refreshes the ticket with `login`.
3. The passthrough command runner calls `AutoLogin()` before non-auth commands.
4. The `login` command strips the p5-only `--save` flag before forwarding arguments to Perforce.

## Storage

The password store writes one credential file per `P4PORT` + `P4USER`.
The file contains AES-256-GCM encrypted data, with a key derived from the current user and machine context plus a random per-file salt.
On Unix-like systems the directory is `0700` and credential files are `0600`.

The `P5_PASSWORD_STORE` environment variable can override the storage directory, which is useful for tests and isolated environments.

## References

- [p4 login](../p4/commands/login.md)
