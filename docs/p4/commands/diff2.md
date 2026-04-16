# p4 diff2


    diff2 -- Diff utility for comparing the content at two depot paths
    (supports files and stream specs). (For comparing workspace content to
    depot content, see p4 diff.)

    p4 diff2 [options] fromFile[rev] toFile[rev]
    p4 diff2 [options] -b branch [[fromFile[rev]] toFile[rev]]
    p4 diff2 [options] [-S stream] [-P parent] [[fromFile[rev]] toFile[rev]]
    p4 diff2 [-d<flags>] -As streamname1[@change1] streamname2[@change2]

    	options: -d<flags> -Od -q -t -u

	'p4 diff2' runs on the server to compare one set of depot files (the
	'source') to another (the 'target').  Source and target file sets
	can be specified on the 'p4 diff2' command line or through a branch
	view.

	With a branch view, fromFile and toFile are optional; fromFile limits
	the scope of the source file set, and toFile limits the scope of the
	target. If only one file argument is given, it is assumed to be
	toFile.

	fromFile and toFile can include revision specifiers; by default, the
	head revisions are diffed.  See 'p4 help revisions' for details
	about specifying file revisions.

	'p4 diff2' precedes each diffed file pair with a header line of the
	following form:

	    ==== source#rev (type) - target#rev (type) ==== summary

	A source or target file shown as '<none>' means there is no file
	at the specified name or revision to pair with its counterpart.
	The summary status is one of the following: 'identical' means file
	contents and types are identical, 'types' means file contents are
	identical but the types are different, and 'content' means file
	contents are different.

	The -b flag makes 'p4 diff2' use a user-defined branch view.  (See
	'p4 help branch'.) The left side of the branch view is the source
	and the right side is the target.

	The -S flag makes 'p4 diff2' use a generated branch view that maps
	a stream (or its underlying real stream) to its parent.  This defaults
	to the current client's stream, if applicable.  -P can be used to
	generate the branch view using a parent stream other than the stream's
	actual parent.

	The -As flag can be used to diff two different stream specifications.
	By default, the head version of a given stream spec is used; if a
	change list number is given, the stream spec version associated with
	the changelist will be used instead. If '@=change' is given, the
	shelved stream spec version associated with the changelist will be
	used instead.

	The -d<flags> modify the output of diffs as follows:

		-dn (RCS)
		-dc[n] (context)
		-ds (summary)
		-du[n] (unified)
		-db (ignore whitespace changes)
		-dw (ignore whitespace)
		-dl (ignore line endings).

	The optional argument to -dc/-du specifies number of context lines.

	The -Od flag limits output to files that differ.

	The -q omits files that have identical content and types and
	suppresses the actual diff for all files.

	The -t flag forces 'p4 diff2' to diff binary files.

	The -u flag uses the GNU diff -u format and displays only files
	that differ. The file names and dates are in Perforce syntax, but
	the output can be used by the patch program.

	See 'p4 help-graph diff2' for information on using this command with
	graph depots.
