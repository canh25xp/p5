# p4 undo


    undo -- Undo a range of revisions

    p4 undo [-n -K] [-c changelist#] file[revRange]

	'p4 undo' opens files in order to undo a set of previously submitted
	changes.  The 'undone' changes remain a part of the history, but the
	new revisions submitted after 'p4 undo' will reverse their effect.

	If a single revision is specified, the specified revision is undone.
	If a revision range is specified, the entire range is undone.

	The workspace files opened by 'p4 undo' will be synced to the revision
	prior to the range being undone, and opened at the most recent undone
	revision.  Files that are opened at a revision prior to the head must
	be resolved prior to submit -- run 'p4 sync' and 'p4 resolve'.

	The -n flag previews the operation without changing any files.

	The -K flag suppresses keyword expansion when updating ktext files on
	the client.

	If -c changelist# is included, files are opened in the specified
	pending changelist instead of the default changelist.

	See 'p4 help-graph undo' for information on using this command with
	graph depots.
