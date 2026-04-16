# p4 revert


    revert -- Discard changes from an opened file or stream spec.

    p4 revert [-a -n -k -K -w -c cl# -C client [-Si]] [--remote=rmt] file ...
    p4 revert -So [-c changelist# -C client]

	Revert an open file to the revision that was synced from the depot,
	discarding any edits or integrations that have been made.  You must
	explicitly specify the files to be reverted.  Files are removed from
	the changelist in which they are open.  Locked files are unlocked.

	The -a flag reverts only files that are open for edit, add, or
	integrate and are unchanged or missing. Files with pending
	integration records are left open. The file arguments are optional
	when -a is specified.

	The -n flag displays a preview of the operation.

	The -k flag marks the file as reverted in server metadata without
	altering files in the client workspace.

	The -K flag suppresses keyword expansion when updating ktext files on
	the client.

	The -w flag causes files that are open for add to be deleted from the
	workspace when they are reverted.

	The -c flag reverts files that are open in the specified changelist.

	The -C flag allows a user to specify the workspace that has the file
	opened rather than defaulting to the current client workspace. When
	this option is used, the '-k' flag is also enabled and the check for
	matching user is disabled. The -C flag requires 'admin' access, which
	is granted by 'p4 protect'.

	The -So flag can be used with '-c change' to revert the client's open
	stream spec.  The -Si flag can be used with a file list to include an
	open stream spec when reverting files.   If a stream spec is open and
	neither -Si nor -So is specified, then the stream spec will excluded
	from the reversion.  (See 'p4 help streamcmds'.)

	The --remote flag is useful for DVCS configurations in which files
	of type +l are in use. 'p4 revert --remote=origin filename' reverts
	the named file in your DVCS server, and additionally, if the file is
	of type +l, releases the global exclusive lock on the file in the
	origin server.

	See 'p4 help-graph revert' for information on using this command with
	graph depots.
