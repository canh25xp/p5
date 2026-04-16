# p4 merge


    merge -- Merge one set of files into another set of files, and/or merge
	one stream spec into another stream spec

    p4 merge [options] [-F] [--from stream] [toFile][revRange]
    p4 merge [options] fromFile[revRange] toFile
    p4 merge [options] [-As | -Af] -S stream [-P parent] [-F] [-r]
                       [toFile[rev] ...]

	options: -c changelist# -K -m max -n -Ob -q

	'p4 merge' merges changes from one set of files (the 'source') into
	another (the 'target'). It is a simplified form of the 'p4 integrate'
	command, similar to 'p4 integrate -Rbd -Or'.

	Using the client workspace as a staging area, 'p4 merge' schedules all
	affected target files to be resolved per changes in the source.
	Target files outside of	the current client view are not affected.
	Source files need not be within the client view.

	'p4 resolve' must be used to resolve all changes.  'p4 submit' commits
	merged files to	the depot.  Unresolved files may not be submitted.
	Merged files can be shelved with 'p4 shelve' and abandoned with
	'p4 revert'.  The commands 'p4 integrated' and 'p4 filelog' display
	merge history.

	When 'p4 merge' schedules a workspace file to be resolved, it leaves
	it read-only. 'p4 resolve' can operate on a read-only file;  for
	other pre-submit changes, 'p4 edit' must be used to make the file
	writable.

	By default, 'p4 merge' merges changes into the current stream from its
	parent, or from another stream specified by the --from flag.  The
	source and target can also be specified on the command line as a
	pair of file paths.  More complex merge mappings can be specified
	via branchspecs as with 'p4 integrate' (see 'p4 help integrate').

	Each file in the target is mapped to a file in the source. Mapping
	adjusts automatically for files that have been moved or renamed, as
	long as 'p4 move' was used to move/rename files.  The scope of source
	and target file sets must include both old-named and new-named files
	for mappings to be adjusted.  Moved source files may schedule moves
	to be resolved in target files.

	revRange is a revision or a revision range that limits the span of
	source history to be probed for unintegrated revisions.  For details
	about revision specifiers, see 'p4 help revisions'.

	The typical use of 'p4 merge' is to merge into the current stream
	from its parent stream (or the stream specified by --from). More
	advanced uses of 'p4 merge' are possible, using the flags as described
	by 'p4 help integ'.

	The -F flag can be used to force merging against a stream's expected
	flow. It can also force the generation of a branch view based on a
	virtual stream; the mapping itself refers to the underlying real
	stream.

	Stream specifications may also be merged along with stream files.
	Any field in the source specification which has an Openable property of
	'propagate' will be merged to the target specification. The -Af and
	-As flags control whether stream files or the stream spec is merged.

		-Af	Perform merge with files only.
		-As	Perform merge with stream specs only.

	Both stream files and the stream spec will be merged if neither -Af
	nor -As is specified.  See 'p4 help streamspecinteg' for more details.

	The -Ob flag causes the base revision (if any) to be displayed along
	with each scheduled resolve.

	The -K flag suppresses keyword expansion when updating ktext files on
	the client.

	The -q flag suppresses normal output messages. Messages regarding
	errors or exceptional conditions are displayed.

	See 'p4 help-graph merge' for information on using this command with
	graph depots.
