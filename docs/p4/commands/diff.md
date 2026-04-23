# p4 diff


    diff -- Diff utility for comparing workspace content to depot content
    (supports files and stream spec). (For comparing two depot paths, see
    p4 diff2.)

    p4 diff [-d<flags> -f -m max -Od -s<flag> -t] [file[rev] ...]
    p4 diff [-d<flags>] -As [streamname[@change]]

	On the client machine, diff a client file against the corresponding
	revision in the depot. The file is compared only if the file is
	opened for edit or a revision is provided. See 'p4 help revisions'
	for details about specifying revisions.

	If the file specification is omitted, all open files are diffed.
	This option can be used to view pending changelists.

	The -As flag can be used to diff an opened stream spec with either
	the head version of the stream spec (the default), or against the
	specified stream spec at either head or the specified change number.
	To diff a shelved version of the specified stream spec, use the
	revision specification '@=change', where 'change' is the shelf
	changelist number.

	The -d<flags> modify the output as follows:
	    -dn (RCS),
	    -dc[n] (context),
	    -ds (summary),
	    -du[n] (unified),
	    -db (ignore whitespace changes),
	    -dw (ignore whitespace),
	    -dl (ignore line endings).
	The optional argument to -dc/-du specifies number of context lines.

	The -f flag diffs every file, regardless of whether they are opened
	or the client has synced the specified revision.  This option can be
	used to verify the contents of the client workspace.

	The -m max flag limits output to the first 'max' number of files,
	unless the -s flag is used, in which case it is ignored.

	The -Od flag limits output to files that differ.

	The -s options lists the files that satisfy the following criteria:

		-sa     Opened files that differ from the revision
			in the depot or are missing.

		-sb     Files that have been opened for integrate, resolved,
			and subsequently modified.

		-sd     Unopened files that are missing on the client.

		-se     Unopened files that differ from the revision
			in the depot.

		-sl	Every unopened file, along with the status of
			'same, 'diff', or 'missing' as compared to the
			corresponding revision in the depot.

		-sr     Opened files that do not differ from the revision in
			the depot.

	Note that if a revision is provided in the file specification, the -s
	options compare the file(s) regardless of whether they are opened
	or the client has synced the specified revision.

	The -t flag forces 'p4 diff' to diff binary files.

	If the environment variable $P4DIFF is set,  the specified diff
	program is launched in place of the default Perforce client diff.
	The -d<flags> option can be used to pass arguments to the diff
	program.  Because the -s flag is only implemented internally, any
	-d<flags> option used with the -s<flag> is ignored. To configure a
	diff program for Unicode files, set the environment variable
	$P4DIFFUNICODE. Specify the file's character set as the first
	argument to the program.

	See 'p4 help-graph diff' for information on using this command with
	graph depots.
