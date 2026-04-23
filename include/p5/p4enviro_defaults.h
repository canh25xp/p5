#pragma once

/// If P4ENVIRO is unset, set the process default to match Perforce on POSIX:
/// a file named `.p4enviro` in the user's home directory. On Windows the stock
/// client uses no file unless P4ENVIRO is set; p5 sets it to %USERPROFILE%\.p4enviro
/// for parity. Call from main() before P4 or Enviro initialization.
void p4enviro_apply_default_if_unset();
