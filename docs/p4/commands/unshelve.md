# p4 unshelve


    unshelve -- Restore shelved files and/or a stream spec from a pending
	change into a workspace

    p4 unshelve -s changelist# [options] [-As| [-Af] file ...]
	Options: [-f -n] [-c changelist#]
	         [-b branch|-S stream [-P parent]]

	'p4 unshelve' retrieves shelved files from the specified pending
	changelist, opens them in a pending changelist and copies them
	to the invoking user's workspace.  Unshelving files from a pending
	changelist is restricted by the user's permissions on the files.
	A successful unshelve operation places the shelved files on the
	user's workspace with the same open action and pending integration
	history as if it had originated from that user and client.

	Unshelving a file over an already opened file is permitted if both
	shelved file and opened file are opened for 'edit'. In a distributed
	environment, the shelf must either be promoted or have been created
	on the same edge server. After unshelving, the workspace file is
	flagged as unresolved, and 'p4 resolve' must be run to resolve the
	differences between the shelved file and the workspace file.

	Unshelving a file opened for 'add' when the file already exists
	in the depot will result in the file being opened for edit. After
	unshelving, the workspace file is flagged as unresolved, and
	'p4 resolve' must be run to resolve the differences between the
	shelved file and the depot file at the head revision. Note that
	unshelving a file opened for 'add' over an already opened file is
	not supported.

	The -s flag specifies the number of the pending changelist that
	contains the shelved files.

	If a file pattern is specified, 'p4 unshelve' unshelves files that
	match the pattern.

	The -A flag specifies whether files and/or the stream spec (if any)
	associated with the shelved changelist should be unshelved to the
	current client; by default, the unshelve command acts on both the
	files (-Af) and the stream spec (-As).  (See 'p4 help streamcmds'.)

	The -b flag specifies a branch spec that the shelved files will be
	mapped through prior to being unshelved, allowing files to be shelved
	in one branch and unshelved in another.  As with unshelving into an
	open file, it may be necessary to run 'p4 resolve'. In a distributed
	environment, an additional requirement is that the shelve was created
	on the same edge server.

	The -S flag uses a generated branch view to map the shelved files
	between the specified stream and its parent stream.  The -P flag
	can be used to generate the view using a parent stream other than
	the actual parent.

	The -c flag specifies the changelist to which files are unshelved.
	By default,  'p4 unshelve' opens shelved files in the default
	changelist.

	The -f flag forces the clobbering of any writeable but unopened files
	that are being unshelved.

	The -n flag previews the operation without changing any files or
	metadata.
