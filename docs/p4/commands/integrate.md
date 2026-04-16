# p4 integrate


    integrate -- Integrate one set of files into another set of files, and/or
	integrate one stream spec into another stream spec

    p4 integrate [options] fromFile[revRange] toFile
    p4 integrate [options] -b branch [-r] [toFile[revRange] ...]
    p4 integrate [options] -b branch -s fromFile[revRange] [toFile ...]
    p4 integrate [options] [-As | -Af] -S stream [-r] [-P parent]
	                         [file[revRange] ...]

	options: -c changelist# -Di -f -h -K -O<flags> -n -m max
	         -R<flags> -q -v

	'p4 integrate' integrates one set of files (the 'source') into
	another (the 'target').

	(See also 'p4 merge' and 'p4 copy', variants of 'p4 integrate' that
	may be easier and more effective for the task at hand.)

	Using the client workspace as a staging area, 'p4 integrate' adds and
	deletes target files per changes in the source, and schedules all
	other affected target files to be resolved.  Target files outside of
	the current client view are not affected. Source files need not be
	within the client view.

	'p4 resolve' must be used to merge file content, and to resolve
	filename and filetype changes. 'p4 submit' commits integrated files
	to the depot.  Unresolved files may not be submitted.  Integrations
	can be shelved with 'p4 shelve' and abandoned with 'p4 revert'.  The
	commands 'p4 integrated' and 'p4 filelog' display integration history.

	When 'p4 integrate' schedules a workspace file to be resolved, it
	leaves it read-only. 'p4 resolve' can operate on a read-only file.
	For other pre-submit changes, 'p4 edit' must be used to make the
	file writable.

	Source and target files can be specified either on the 'p4 integrate'
	command line or through a branch view. On the command line, fromFile
	is the source file set and toFile is the target file set.  With a
	branch view, one or more toFile arguments can be given to limit the
	scope of the target file set.

	revRange is a revision or a revision range that limits the span of
	source history to be probed for unintegrated revisions.  revRange
	can be used on fromFile, or on toFile, but not on both.  When used on
	toFile, it refers to source revisions, not to target revisions.  For
	details about revision specifiers, see 'p4 help revisions'.

	The -S flag makes 'p4 integrate' use a generated branch view that maps
	a stream (or its underlying real stream) to its parent.  With -r, the
	direction of the mapping is reversed.  -P can be used to generate the
	branch view using a parent stream other than the stream's actual
	parent.  Note that to submit integrated stream files, the current
	client must be switched to the target stream, or to a virtual child
	stream of the target stream.

	Stream specifications may also be integrated along with stream files.
	Any field in the source specification which has an Openable property of
	'propagate' will be integrated to the target specification. The -Af and
	-As flags control whether stream files or the stream spec is
	integrated.

		-Af	Perform integration with files only.
		-As	Perform integration with stream specs only.

	Both stream files and the stream spec will be integrated if neither -Af
	nor -As is specified.  See 'p4 help streamspecinteg' for more details.

	The -b flag makes 'p4 integrate' use a user-defined branch view.
	(See 'p4 help branch'.) The source is the left side of the branch view
	and the target is the right side. With -r, the direction is reversed.

	The -s flag can be used with -b to cause fromFile to be treated as
	the source, and both sides of the branch view to be treated as the
	target, per the branch view mapping.  Optional toFile arguments may
	be given to further restrict the scope of the target file set.  The
	-r flag is ignored when -s is used.

	Note that 'p4 integrate' automatically adusts source-to-target
	mappings for moved and renamed files.  (Adjustment occurs only if
	the 'p4 move' command was used to move/rename files.) The scope of
	source and target file sets must include both the old-named and the
	new-named files for mappings to be adjusted.  A filename resolve is
	scheduled for each remapped file to allow the target to be moved to
	match the source.

	The -f flag forces integrate to ignore integration history and treat
	all source revisions as unintegrated. It is meant to be used with
	revRange to force reintegration of specific, previously integrated
	revisions.

	The -O flags cause more information to be output for each file opened:

		-Ob	Show the base revision for the merge (if any).
		-Or	Show the resolve(s) that are being scheduled.

	The -R flags modify the way resolves are scheduled:

		-Rb	Schedules 'branch resolves' instead of branching new
			target files automatically.

		-Rd	Schedules 'delete resolves' instead of deleting
			target files automatically.

		-Rs	Skips cherry-picked revisions already integrated.
			This can improve merge results, but can also cause
			multiple resolves per file to be scheduled.

	The -Di flag modifies the way deleted revisions are treated.  If the
	source file has been deleted and re-added, revisions that precede
	the deletion will be considered to be part of the same source file.
	By default, re-added files are considered to be unrelated to the
	files of the same name that preceded them. When the source file has
	been moved or renamed, the move/add and move/delete revisions are
	propagated as branch and delete revisions.

	The -h flag leaves the target files at the revision currently synced
	to the client (the '#have' revision). By default, target files are
	automatically synced to the head revision by 'p4 integrate'.

	The -K flag suppresses keyword expansion when updating ktext files on
	the client.

	The -m flag limits integration to the first 'max' number of files.

	The -n flag displays a preview of integration, without actually
	doing anything.

	The -q flag suppresses normal output messages. Messages regarding
	errors or exceptional conditions are displayed.

	If -c changelist# is specified, the files are opened in the
	designated numbered pending changelist instead of the 'default'
	changelist.

	The -v flag causes a 'virtual' integration that does not modify
	client workspace files unless target files need to be resolved.
	After submitting a virtual integration, 'p4 sync' can be used to
	update the workspace.
