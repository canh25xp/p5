# p4 shelve


    shelve -- Store files (or an open stream spec) from a pending changelist
    into the depot

    p4 shelve [-p] [--parallel=threads=N[,batch=N][,min=N]]
              [-As | [-Af] file ...]
    p4 shelve [-a option] [-p] -i [-f | -r] [-As | -Af]
              [--parallel=threads=N[,batch=N][,min=N]]
    p4 shelve [-a option] [-p] -r -c changelist# [-As | -Af]
              [--parallel=threads=N[,batch=N][,min=N]]
    p4 shelve [-a option] [-p] -c changelist# [-f]
              [--parallel=threads=N[,batch=N][,min=N]]
              [-As | [-Af] file ...]
    p4 shelve -d -c changelist# [-f] [-As | [-Af] file ...]

	'p4 shelve' creates, modifies or deletes shelved files in a pending
	changelist. Shelved files remain in the depot until they are deleted
	(using 'p4 shelve -d') or replaced by subsequent shelve commands.
	After 'p4 shelve', the user can revert the files and restore them
	later using 'p4 unshelve'.  Other users can 'p4 unshelve' the stored
	files into their own workspaces.

	Files that have been shelved can be accessed by the 'p4 diff',
	'p4 diff2', 'p4 files' and 'p4 print' commands using the revision
	specification '@=change', where 'change' is the pending changelist
	number.

	By default, 'p4 shelve' creates a changelist, adds files from the
	user's default changelist, then shelves those files in the depot.
	The user is presented with a text changelist form displayed using
	the editor configured using the $P4EDITOR environment variable.

	If a file pattern is specified, 'p4 shelve' shelves the files that
	match the pattern. When files are opened in the default changelist,
	only a single file pattern can be specified when creating a shelf;
	a file pattern containing wildcards can be used to match and shelve
	multiple files.

	The -Af flag specifies that only files should be shelved with this
	changelist.  The -As flag specifies that only an opened stream
	specification should be shelved with this change list.  By default,
	if the stream spec is open and neither -Af nor -As is given, the
	stream specification will also be included with any shelved
	files.  (See 'p4 help streamcmds'.)

	The -i flag reads the pending changelist specification with shelved
	files from the standard input.  The user's editor is not invoked.
	To modify an existing changelist with shelved files, specify the
	changelist number using the -c flag.

	The -c flag specifies the pending changelist that contains shelved
	files to be created, deleted, or modified. Only the user and client
	of the pending changelist can add or modify its shelved files. Any
	files specified by the file pattern must already be opened in the
	specified changelist; use 'p4 reopen' to move an opened file from
	one changelist to another.

	The -f (force) flag must be used with the -c or -i flag to overwrite
	any existing shelved files in a pending changelist.

	The -r flag (used with -c or -i) enables you to replace all shelved
	files in that changelist with the files opened in your own workspace
	at that changelist number.  Previously shelved files will be deleted.
	Only the user and client workspace of the pending changelist can
	replace its shelved files.

	The -a flag enables you to handle unchanged files similarly to some
	client submit options, namely 'submitunchanged' and 'leaveunchanged'.
	The default behavior of shelving all files corresponds to the
	'submitunchanged' option. The 'leaveunchanged' option only shelves
	changed files, and then leaves the files opened in the pending
	changelist on the client.

	The -d flag (used with -c) deletes the shelved files in the specified
	changelist so that they can no longer be unshelved.  By default, only
	the user and client of the pending changelist can delete its shelved
	files. A user with 'admin' access can delete shelved files by including
	the -f flag to force the operation.

	If the shelved changelist includes a stream spec, by default it is
	deleted when all files have been deleted; the -As flag forces the
	the stream spec to be deleted even if files remain.

	The -p flag promotes a shelved change from an edge server to a commit
	server where it can be accessed by other edge servers participating
	in the distributed configuration.  Once a shelved change has been
	promoted, all subsequent local modifications to the shelf are also
	pushed to the commit server and remain until the shelf is deleted.
	Once a shelf has been created, the combination of flags '-p -c' will
	promote the shelf without modification unless '-f' or '-r' are also
	used to update the shelved files before promotion.

	The --parallel flag specifies options for parallel file transfer. If
	parallel file transfer has been enabled by setting the
	net.parallel.max configurable, and if there are sufficient resources
	across the system, a shelve command may execute more rapidly by
	transferring multiple files in parallel. Specify threads=N to request
	files be sent concurrently, using N independent network connections.
	The N threads grab work in batches: specify batch=N to control the
	number of files in a batch. A shelve that is too small will not
	initiate parallel file transfers: specify min=N to control the
	minimum number of files in a parallel shelve. Promoted shelves
	require an additional file transfer from the Edge to Commit Server.
	Parallel pull threads for this transfer are only used if the
	ExternalAddress field is set in its Edge Server spec and pull threads
	can be used on the Commit Server. This transfer using pull threads
	is currently not supported on Windows platforms.

	Auto parallel shelve may be enabled by setting the
	net.parallel.shelve.threads configurable to the desired number
	of threads to be used for all shelve commands. This value must be
	less than or equal to the value of net.parallel.max. Other
	net.parallel.shelve.* configurables may be specified as well, but
	are not required. See 'p4 help configurables' to see the options
	and their defaults. Auto parallel shelve is turned off by unsetting
	the net.parallel.shelve.threads configurable. A user may override
	the configured auto parallel shelve options on the command line,
	or may disable it via 'p4 shelve --parallel=0'.
