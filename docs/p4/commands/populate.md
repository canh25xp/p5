# p4 populate


    populate -- Branch a set of files as a one-step operation

    p4 populate [options] fromFile[rev] toFile
    p4 populate [options] -b branch [-r] [toFile[rev]]
    p4 populate [options] -b branch -s fromFile[rev] [toFile]
    p4 populate [options] -S stream [-P parent] [-r] [toFile[rev]]

	options: -d description -f -m max -n -o

	'p4 populate' branches a set of files (the 'source') into another depot
	location (the 'target') in a single step.  The new files are created
	immediately, without requiring a 'p4 submit' or a client workspace.

	The target must be completely empty of files, including deleted files.

	Source and target files (fromFile and toFile) can be specified on
	the command line or through a branch view.  On the command line,
	fromFile is the source file set and toFile is the target file set.
	With a branch view, a toFile argument can be given to limit the scope
	of the target file set.

	A revision specifier can be used to select the revision to branch; by
	default, the head revision is branched. The revision specifier can be
	used on fromFile, or on toFile, but not on both.  When used on toFile,
	it refers to source revisions, not to target revisions.  A range may
	not be used as a revision specifier.  For revision syntax, see 'p4
	help revisions'.

	Use 'p4 populate -r -S' to branch a child stream from its parent.
	The -S flag makes 'p4 populate' use a generated branch view that maps
	the stream (or its underlying real stream) to its parent.  With -r,
	the direction of the mapping is reversed.  -P can be used to generate
	the branch view using a parent stream other than the stream's actual
	parent.

	The -b flag makes 'p4 populate' use a user-defined branch view.  (See
	'p4 help branch'.) The source is the left side of the branch view
	and the target is the right side. With -r, the direction is reversed.

	The -s flag can be used with -b to cause fromFile to be treated as
	the source, and both sides of the user-defined branch view to be
	treated as the target, per the branch view mapping.  An optional
	toFile argument may be given to further restrict the scope of the
	target file set.  -r is ignored when -s is used.

	The -d flag specifies a description for the submitted changelist.

	The -f flag forces deleted files to be branched into the target.
	By default, deleted files are treated as nonexistent and simply
	skipped.

	The -n flag displays a preview of the operation, without actually
	doing anything.

	The -o flag displays a list of files created by the populate command.

	The -m flag limits the actions to the first 'max' number of files.
