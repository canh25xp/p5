# Auto login

## Problem

Currently `p4 login` will ask for user password and generate a ticket that valid for 12 hours (Maybe defined by p4 server).
After ticket expired, users must re-login, .i.e re-enter password.
Doing that each day could be a little of a pain.
Especially in automated tasks where p4 may blocking the flow by wait for using login.

## Solution

Implement a **remember password** feature for `p5` to perform auto login each day.
Password should NOT store as clear text on machine and should be encrypted.

## Implementation

1. Implement savePassword() and getPassword() features.
2. Implement autoLogin() to perform login using savePassword()
3. Implement isTicketExpired() method to check if autoLogin() needed
4. Add login commands with --save flag to save password.

## References

- [p4 login](../p4/commands/login.md)
