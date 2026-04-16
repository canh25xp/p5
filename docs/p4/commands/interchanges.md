# p4 interchanges


    interchanges -- Report changes not yet integrated

    p4 interchanges [options] fromFile[revRange] toFile
    p4 interchanges [options] -b branch [toFile[revRange] ...]
    p4 interchanges [options] -b branch -s fromFile[revRange] [toFile ...]
    p4 interchanges [options] -S stream [-P parent] [file[revRange] ...]

	options: -f -l -r -t -u -F

	'p4 interchanges' lists changes that have not been integrated from
	a set of source files to a set of target files.

	The -f option lists the individual files that require integration.
	For partially integrated changelists, files might be listed even if
	they have been integrated individually.

	The -l flag includes the full text of the changelist descriptions.

	The -t flag displays the time as well as the date.

	The -b flag makes 'p4 interchanges' use a user-defined branch view.
	(See 'p4 help branch'.) The source is the left side of the branch
	view and the target is the right side.  Used with the -r flag, the
	direction is reversed.

	The -s flag can be used with -b to cause fromFile to be treated as
	the source, and both sides of the branch view to be treated as the
	target, per the branch view mapping.  Optional toFile arguments may
	be given to further restrict the scope of the target file set.  The
	-r flag is ignored when -s is used.

	The -u flag limits results to those submitted by a particular user.

	The -S flag causes 'p4 interchanges' to use a generated branch view
	that maps a stream to its parent.  With -r, the direction of the
	mapping is reversed.  -P can be used to generate the branch view
	using a parent stream other than the stream's actual parent.  The -S
	flag also makes 'p4 interchanges' respect a stream's flow.

	The -F flag can be used with -S to force 'p4 interchanges' to ignore
	a stream's expected flow.  It can also force it to generate a branch
	view based on a virtual stream; the mapping itself refers to the
	underlying real stream.
