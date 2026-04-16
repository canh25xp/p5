# Perforce client usage:

```sh
p4 [options] command [--explain] [arg ...]
p4 -V
```

	Many command flags have both short-form and long-form syntax. Specify
	the --explain flag to display additional information about the flags
	accepted by this command. For example, 'p4 sync --explain' to display
	all the options supported by sync, or 'p4 edit --explain -k' to
	display information about the -k flag to edit.

	options:
	    -b batchsize -c client -C charset -d dir -H host
	    -I -G -L language -p port -P pass -q -r retries -s
	    -Q charset -u user -x file -z tag

	The -b batchsize specifies a batch size (number of arguments) to
	use when processing a command from a file '-x' (default 128).

	The -c flag specifies the client name, overriding the value of
	$P4CLIENT in the environment and the default (the hostname).

	The -C flag specifies the client's character set, overriding the
	value of $P4CHARSET in the environment.  See 'p4 help charset'
	for more information.

	The -d flag specifies the current directory, overriding the value of
	$PWD in the environment and the default (the current directory).

	The -G flag formats all output (and batch input for form commands
	with -i) as marshalled Python dictionary objects.

	The -Mj flag formats all output (and batch input for form commands
	with -i) as strict line-delimited JSON.  Non-UTF8 data will be
	converted to the Unicode replacement character.

	The -ztag flag is required for all marshalled output flags.

	The -H flag specifies the host name, overriding the value of
	$P4HOST in the environment and the default (the hostname).

	The -I flag specifies that progress indicators, if available,
	are desired.  Progress indicators are available for the commands:
	'p4 -I submit', 'p4 -I sync -q', 'p4 -I switch', 'p4 clone',
	'p4 -I fetch', 'p4 -I push', 'p4 -I zip', and 'p4 -I unzip'.
	This flag is not compatible with the -s, -e, -G options.

	The -L flag specifies the language for text messages from the
	server, overriding the value of $P4LANGUAGE in the environment.
	Setting this option only works if the administrator has loaded
	support for non-English messages into the server database.

	The -p flag specifies the server's listen address, overriding the
	value of $P4PORT in the environment and the default (perforce:1666).

	The -P flag specifies the password, overriding the value of
	$P4PASSWD in the environment.

	The -q flag suppresses all info messages.

	The -Q flag specifies the client's command character set,
	overriding the value of $P4COMMANDCHARSET in the environment.
	See 'p4 help charset' for more information.

	The -r flag specifies the number of times a sync command should be
	retried if the network times out (takes longer than N seconds to
	respond to a single I/O operation) during sync command execution.
	For example:
           p4 -r4 -vnet.maxwait=300 sync //...
	specifies to retry this 'sync' up to 4 times, should any single
	network operation take longer than 5 minutes to complete. The -r
	flag should not be used on a command which reads from stdin.

	The -s flag causes the p4 command line client program to prefix
	each line of output with a tag (error, warning, info, text, exit)
	to make it easier to use for scripting.

	The -u flag specifies the user name, overriding the value of
	$P4USER, $USER, and $USERNAME in the environment.

	The -x flag instructs p4 to read arguments, one per line, from the
	specified file.  If you specify '-', standard input is read.

	The -V flag displays the version of the p4 client command and exits.

	The -z tag option returns output of reporting commands in the
	format returned by 'p4 fstat'.

	The --no-script flag disables client-side Extensions.
