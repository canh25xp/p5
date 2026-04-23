# p4 filelog


    filelog -- List revision history of files

    p4 filelog [-c changelist# -h -i -l -L -t -m max -p -s] file[revRange] ...

	List the revision history of the specified files, from the most
	recent revision to the first.  If the file specification includes
	a revision, the command lists revisions at or prior to the specified
	revision.  If the file specification includes a revision range,
	the command lists only the specified revisions.
	See 'p4 help revisions' for details.

	The -c changelist# flag displays files submitted at the specified
	changelist number.

	The -i flag includes inherited file history. If a file was created by
	branching (using 'p4 integrate'), filelog lists the revisions of the
	file's ancestors up to the branch points that led to the specified
	revision.  File history inherited by renaming (using 'p4 move') is
	always displayed regardless of whether -i is specified.

	The -h flag displays file content history instead of file name
	history.  The list includes revisions of other files that were
	branched or copied (using 'p4 integrate' and 'p4 resolve -at') to
	the specified revision.  Revisions that were replaced by copying
	or branching are omitted, even if they are part of the history of
	the specified revision.

	The -t flag displays the time as well as the date.

	The -l flag lists the full text of the changelist descriptions.

	The -L flag lists the full text of the changelist descriptions,
	truncated to 250 characters if longer.

	The -m max displays at most 'max' revisions per file of the file[rev]
	argument specified.

	The -p flag is used in conjunction with the '-h' flag to prevent
	filelog from following content of promoted task streams. This flag
	is useful when there are many child task streams branched from the
	file argument supplied.

	The -s flag displays a shortened form of filelog that omits
	non-contributory integrations.

	See 'p4 help-graph filelog' for information on using this command with
	graph depots.
