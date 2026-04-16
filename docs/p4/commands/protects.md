# p4 protects


    protects -- Display protections defined for a specified user and path

    p4 protects [-s spec] [-a | -g group | -u user]
                [-h host | -H] [-m] [-S | -A] [file ...]
    p4 protects -M [-g group | -u user] [-h host | -H] [file ...]

	'p4 protects' displays the lines from the protections table that
	apply to the current user.  The protections table is managed using
	the 'p4 protect' command.

	If the -a flag is specified, protection lines for all users are
	displayed.  If the -g group flag or -u user flag is specified,
	protection lines for that group or user are displayed.

	If the -h host flag is specified, the protection lines that apply
	to the specified host (IP address) are displayed.

	If the -H flag is specified, the protection lines that apply to the
	current client's host (IP address) are displayed.

	If the -m flag is given, a single word summary of the maximum
	access level is reported. Note that this summary does not take
	exclusions or the specified file path into account.

	If the -M flag is given, a single word summary of the maximum
	access level is reported. Unlike '-m' it does take into account
	exclusions and the specified file path.

	If the -s spec flag is specified and a spec depot has been defined
	then the command runs using the contents of the file in the spec
	depot rather than the current protections table. This allows the
	'super' user back-in-time access to how permission would have
	behaved in a previous version of the protections spec.

	By default 'p4 protects' reports only file permissions.
	If the -S flag is given, report only stream spec protection lines.
	If the -A flag is given, report file and stream spec protection lines.

	If the file argument is specified, protection lines that apply to
	the specified files are displayed.

	The -a/-g/-u flags require 'super' access granted by 'p4 protect'.
