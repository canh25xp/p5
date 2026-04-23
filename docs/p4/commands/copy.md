# p4 copy


    copy -- Copy one set of files into another set of files, and/or copy
	one stream spec into another stream spec

    p4 copy [options] fromFile[rev] toFile
    p4 copy [options] -b branch [-r] [toFile[rev] ...]
    p4 copy [options] -b branch -s fromFile[rev] [toFile ...]
    p4 copy [options] [-As | -Af] -S stream [-P parent] [-F] [-r]
                      [toFile[rev] ...]

	options: -c changelist# -f -K -n -v -m max -q

	'p4 copy' copies one set of files (the 'source') into another (the
	'target').

	Using the client workspace as a staging area, 'p4 copy' makes the
	target identical to the source by branching, replacing, or deleting
	files.  'p4 submit' submits copied files to the depot. 'p4 revert'
	can be used to revert copied files instead of submitting them.  The
	history of copied files can be shown with 'p4 filelog' or 'p4
	integrated'.

	Target files that are already identical to the source, or that are
	outside of the client view, are not affected by 'p4 copy'. Opened,
	non-identical target files cause 'p4 copy' to exit with a warning.
	When 'p4 copy' creates or modifies files in the workspace, it leaves
	them read-only; 'p4 edit' can make them writable.  Files opened by
	'p4 copy' do not need to be resolved.

	Source and target files (fromFile and toFile) can be specified on
	the 'p4 copy' command line or through a branch view. On the command
	line, fromFile is the source file set and toFile is the target file
	set.  With a branch view, one or more toFile arguments can be given
	to limit the scope of the target file set.

	A revision specifier can be used to select the revision to copy; by
	default, the head revision is copied. The revision specifier can be
	used on fromFile, or on toFile, but not on both.  When used on toFile,
	it refers to source revisions, not to target revisions.  A range may
	not be used as a revision specifier.  For revision syntax, see 'p4
	help revisions'.

	The -S flag makes 'p4 copy' copy from a stream to its parent.
	Use -r with -S to reverse the copy direction.  Note that to submit
	copied stream files, the current client must be switched to the
	target stream, or to a virtual child stream of the target stream.
	The -S flag causes 'p4 copy' to use a generated branch view that
	maps the stream to its parent.  -P can be used to generate the
	branch view using a parent stream other than the stream's actual
	parent.  The -S flag also makes 'p4 copy' respect a stream's flow.

	The -F flag can be used with -S to force copying against a stream's
	expected flow. It can also force -S to generate a branch view based
	on a virtual stream; the mapping itself refers to the underlying
	real stream.

	Stream specifications may also be copied along with stream files.
	Any field in the source specification which has an Openable property of
	'propagate' will be copied to the target specification. The -Af and
	-As flags control whether stream files or the stream spec is copied.

		-Af	Perform copy with files only.
		-As	Perform copy with the stream spec only.

	Both stream files and the stream spec will be copied if neither -Af
	nor -As is specified.  See 'p4 help streamspecinteg' for more details.

	The -b flag makes 'p4 copy' use a user-defined branch view.  (See
	'p4 help branch'.) The source is the left side of the branch view
	and the target is the right side. With -r, the direction is reversed.

	The -s flag can be used with -b to cause fromFile to be treated as
	the source, and both sides of the user-defined branch view to be
	treated as the target, per the branch view mapping.  Optional toFile
	arguments may be given to further restrict the scope of the target
	file set. -r is ignored when -s is used.

	The -c changelist# flag opens files in the designated (numbered)
	pending changelist instead of the default changelist.

	The -f flag forces the creation of extra revisions in order to
	explicitly record that files have been copied.  Deleted source files
	will be copied if they do not exist in the target, and files that are
	already identical will be copied if they are not connected by existing
	integration records.

	The -K flag suppresses keyword expansion when updating ktext files on
	the client.

	The -n flag displays a preview of the copy, without actually doing
	anything.

	The -m flag limits the actions to the first 'max' number of files.

	The -q flag suppresses normal output messages. Messages regarding
	errors or exceptional conditions are displayed.

	The -v flag causes a 'virtual' copy that does not modify client
	workspace files.  After submitting a virtual integration, 'p4 sync'
	can be used to update the workspace.
