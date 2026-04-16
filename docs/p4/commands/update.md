# p4 update


    sync -- Synchronize the client with its view of the depot
    flush -- synonym for 'sync -k'
    update -- synonym for 'sync -s'

    p4 sync [-E -f -L -n -N -k -K -q -r] [-m max]
            [--use-stream-change=N]
            [--parallel=threads=N[,batch=N][,batchsize=N][,min=N][,minsize=N]]
            [file[revRange] ...]
    p4 sync [-E -L -n -N -K -q -s] [-m max]
            [--use-stream-change=N]
            [--parallel=threads=N[,batch=N][,batchsize=N][,min=N][,minsize=N]]
            [file[revRange] ...]
    p4 sync [-E -L -n -N -K -p -q] [-m max]
            [--use-stream-change=N]
            [--parallel=threads=N[,batch=N][,batchsize=N][,min=N][,minsize=N]]
            [file[revRange] ...]

	Sync updates the client workspace to reflect its current view (if
	it has changed) and the current contents of the depot (if it has
	changed). The client view maps client and depot file names and
	locations.

	Sync adds files that are in the client view and have not been
	retrieved before.  Sync deletes previously retrieved files that
	are no longer in the client view or have been deleted from the
	depot.  Sync updates files that are still in the client view and
	have been updated in the depot.

	By default, sync affects all files in the client workspace. If file
	arguments are given, sync limits its operation to those files.
	The file arguments can contain wildcards.

	If the file argument includes a revision specifier, then the given
	revision is retrieved.  Normally, the head revision is retrieved.

	If the file argument includes a revision range specification,
	only files selected by the revision range are updated, and the
	highest revision in the range is used.

	See 'p4 help revisions' for help specifying revisions or ranges.

	Normally, sync does not overwrite workspace files that the user has
	manually made writable.  Setting the 'clobber' option in the
	client specification disables this safety check.

	The -f flag forces resynchronization even if the client already
	has the file, and overwriting any writable files.  This flag doesn't
	affect open files.

	The -L flag can be used with multiple file arguments that are in
	full depot syntax and include a valid revision number. When this
	flag is used the arguments are processed together by building an
	internal table similar to a label. This file list processing is
	significantly faster than having to call the internal query engine
	for each individual file argument. However, the file argument syntax
	is strict and the command will not run if an error is encountered.

	The -n flag previews the operation without updating the workspace.

	The -N flag also previews the operation without updating the
	workspace, but reports only a summary of the work the sync would do.

	The -k flag updates server metadata without syncing files. It is
	intended to enable you to ensure that the server correctly reflects
	the state of files in the workspace while avoiding a large data
	transfer. Caution: an erroneous update can cause the server to
	incorrectly reflect the state of the workspace.

	The -K flag suppresses keyword expansion.

	The -p flag populates the client workspace, but does not update the
	server to reflect those updates.  Any file that is already synced or
	opened will be bypassed with a warning message.  This option is very
	useful for build clients or when publishing content without the
	need to track the state of the client workspace.

	The -q flag suppresses normal output messages. Messages regarding
	errors or exceptional conditions are not suppressed.

	The -r flag allows open files which are mapped to new locations
	in the depot to be reopened in the new location.  By default, open
	workspace files remain associated with the depot files that they were
	originally opened as.

	The -s flag adds a safety check before sending content to the client
	workspace.  This check uses MD5 digests to compare the content on the
	clients workspace against content that was last synced.  If the file
	has been modified outside of Perforce's control then an error message
	is displayed and the file is not overwritten.  This check adds some
	extra processing which will affect the performance of the operation.
	Clients with 'allwrite' and 'noclobber' set do this check by default.

	The -m flag limits sync to the first 'max' number of files. This
	option is useful in conjunction with tagged output and the '-n'
	flag, to preview how many files will be synced without transferring
	all the file data.

	The -E flag verifies that any changelists specified in the revRange
	are existing, submitted changes before performing the sync.

	The --parallel flag specifies options for parallel file transfer. If
	your administrator has enabled parallel file transfer by setting the
	net.parallel.max configurable, and if there are sufficient resources
	across the system, a sync command may execute more rapidly by
	transferring multiple files in parallel. Specify threads=N to request
	files be sent concurrently, using N independent network connections.
	The N threads grab work in batches; specify batch=N to control the
	number of files in a batch and/or batchsize=N to control the number of
	bytes in a batch. A sync that is too small will not initiate parallel
	file transfers; specify min=N to control the minimum number of files
	in a parallel sync and/or minsize=N to control the minimum number of
	bytes in a parallel sync. Requesting progress indicators causes the
	--parallel flag to be ignored.

	Auto parallel sync may be enabled by setting the net.parallel.threads
	configurable to the desired number of threads to be used by all sync
	commands. This value must be less than or equal to the value of
	net.parallel.max. Other net.parallel.* configurables may be specified
	as well, but are not required. See 'p4 help configurables' to see
	the options and their defaults. Auto parallel sync is turned off by
	unsetting the net.parallel.threads configurable. A user may override
	the configured auto parallel sync options on the command line, or may
	disable it via 'p4 sync --parallel=0'.

	The --use-stream-change flag specifies options for syncing stream
	clients using the stream view at or before change N.  If N=0, the
	current head version of the stream spec view is used.  If the
	StreamAtChange field is set in the client specification, then that
	value overrules any --use-stream-change value.

	If the configurable dm.sync.streamchange is set 1, then the
	--use-stream-change flag is used implicitly.  If a change specifier
	is used on the file arguments, then the stream view is derived from the
	stream spec version at or before the change specifier.  If multiple
	file arguments are used with different change specifiers, the greatest
	change number is used.  If no change specifiers are used, or at least
	one file in the filelist has no change specifier, then the stream view
	comes from the head version of the stream spec.  The
	--use-stream-change flag may also be used to override any change
	specifier on a file argument.

	See 'p4 help-graph sync' for information on using this command with
	graph depots.
