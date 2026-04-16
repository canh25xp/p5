# p4 submit


    submit -- Submit open files and/or open stream spec to the depot

    p4 submit [-r -s -f option -K [-b | --noretransfer 0|1]]
              [--parallel=threads=N[,batch=N][,min=N]]
    p4 submit [-r -s -f option -K -b]
              [--parallel=threads=N[,batch=N][,min=N]]
              file ...
    p4 submit [-r -f option -K -b] [-So|-Sx] -d description
              [--parallel=threads=N[,batch=N][,min=N]]
    p4 submit [-r -f option -K -b] [-Sx] -d description
              [--parallel=threads=N[,batch=N][,min=N]]
              file ...
    p4 submit [-r -f option -K [-b | --noretransfer 0|1]] -c changelist#
              [--parallel=threads=N[,batch=N][,min=N]]
    p4 submit -e shelvedChange# [-b]
              [--parallel=threads=N[,batch=N][,min=N]]
    p4 submit -i [-r -s -f option -K -b]
              [--parallel=threads=N[,batch=N][,min=N]]

	'p4 submit' commits a pending changelist and the files it contains to
	the depot, and/or submit an open stream spec.

	By default, 'p4 submit' attempts to submit all files in the 'default'
	changelist.  Submit displays a dialog where you enter a description
	of the change and, optionally, delete files from the list of files
	to be checked in.

	To add files to a changelist before submitting, use any of the
	commands that open client workspace files: 'p4 add', 'p4 edit',
	etc.

	If the file parameter is specified, only files in the default
	changelist that match the pattern are submitted.

	Files in a stream path can be submitted only by client workspaces
	dedicated to the stream. See 'p4 help client'.

	Before committing a changelist, 'p4 submit' locks all the files being
	submitted. If any file cannot be locked or submitted, the files are
	left open in a numbered pending changelist. By default, the files in
	a failed submit operation are left locked unless the
	submit.unlocklocked configurable is set. Files are unlocked even if
	they were manually locked prior to submit if submit fails when
	submit.unlocklocked is set. 'p4 opened' shows unsubmitted files
	and their changelists.

	Submit is atomic: if the operation succeeds, all files are updated
	in the depot. If the submit fails, no depot files are updated.

	If submit fails, some or all of the files may have been copied to
	the server. By default, retrying a failed submit will transfer all of
	the files again unless the submit.noretransfer configurable is set.
	If submit.noretransfer is set to 1, submit uses digest comparisons to
	to detect if the files have already been transferred in order to
	avoid file re-transfer when retrying a failed submit.

	The --noretransfer flag is used to override the submit.noretransfer
	configurable so the user can choose the preferred re-transfer
	behavior during the current submit operation.

	The -So and -Sx flags can be used to control whether an open stream
	spec in the default changelist will be submitted.  Use -So with no file
	list to submit only the open stream spec in the default changelist.
	Use -Sx with a file list to exclude the open stream spec when
	submitting files in the default changelist; use -Sx with no file list
	to submit all files without submitting the open stream spec.

	By default, if both the -So and -Sx flags are omitted, then an open
	stream spec will be submitted along with any files in the given
	filelist.  (See 'p4 help streamcmds'.)

	The -c flag submits the specified pending changelist instead of the
	default changelist. Additional changelists can be created manually,
	using the 'p4 change' command, or automatically as the result of a
	failed attempt to submit the default changelist.

	The -e flag submits a shelved changelist without transferring files
	or modifying the workspace. The shelved change must be owned by
	the person submitting the change, but the client may be different.
	However, files shelved to a stream target may only be submitted by
	a stream client that is mapped to the target stream. In addition,
	files shelved to a non-stream target cannot be submitted by a stream
	client. To submit a shelved change, all files in the shelved change
	must be up to date and resolved. No files may be open in any workspace
	at the same change number. Client submit options (ie revertUnchanged,
	etc) will be ignored. If the submit is successful, the shelved change
	and files are cleaned up, and are no longer available to be unshelved
	or submitted.

	The -d flag passes a description into the specified changelist rather
	than displaying the changelist dialog for manual editing. This option
	is useful for scripting, but does not allow you to add jobs or modify
	the default changelist.

	The -f flag enables you to override submit options that are configured
	for the client that is submitting the changelist.  This flag overrides
	the -r (reopen)flag, if it is specified.  See 'p4 help client' for
	details about submit options.

	The -K flag suppresses keyword expansion when updating ktext files on
	the client after submit.

	The -i flag reads a changelist specification from the standard input.
	The user's editor is not invoked.

	The -r flag reopens submitted files in the default changelist after
	submission.

	The -s flag extends the list of jobs to include the fix status
	for each job, which becomes the job's status when the changelist
	is committed.  See 'p4 help change' for details.

	The --parallel flag specifies options for parallel file transfer. If
	parallel file transfer has been enabled by setting the
	net.parallel.max configurable, and if there are sufficient resources
	across the system, a submit command may execute more rapidly by
	transferring multiple files in parallel. Specify threads=N to request
	files be sent concurrently, using N independent network connections.
	The N threads grab work in batches; specify batch=N to control the
	number of files in a batch. A submit that is too small will not
	initiate parallel file transfers; specify min=N to control the
	minimum number of files in a parallel submit. Requesting progress
	indicators causes the --parallel flag to be ignored.

	Using --parallel from an edge server allows parallel file transfer
	from the edge server to the commit server. This uses standard pull
	threads to transfer the files. Note that an administrator must ensure
	that pull threads can be run on the commit server. The edge server ip
 	address and port number	used by the commit server to connect to the
	edge server must be specified in the ExternalAddress field of the
	edge server spec. Note that parallel submit is not supported from an
	edge server if the commit server is on a Windows platform. In this
	case --parallel	or auto	parallel submit will be silently ignored, and
	the submit will run without using parallel threads.

	Auto parallel submit may be enabled by setting the
	net.parallel.submit.threads configurable to the desired number
	of threads to be used for all submit commands. This value must be
	less than or equal to the value of net.parallel.max. Other
	net.parallel.submit.* configurables may be specified as well, but
	are not required. See 'p4 help configurables' to see the options
	and their defaults. Auto parallel submit is turned off by unsetting
	the net.parallel.submit.threads configurable. A user may override
	the configured auto parallel submit options on the command line,
	or may disable it via 'p4 submit --parallel=0`.

	An edge-content trigger can be used to transfer archive files from
	an Edge to its Commit Server using an alternative file transfer
	mechanism. This option is only used in a distributed environment and
	is not supported for RCS storage. The configurable pull.trigger.dir
	must be set to a location to write temporary files. The configurable
	rpl.submit.nocopy must be set to 1 to suppress default Perforce
	archive file transfer to the Commit Server. The trigger must use
	'fstat -Ob' to get the list of files to transfer, and it must contain
	the file transfer commands.

	The -b flag can be used on an edge server to enable archive transfer
	from the edge to the commit server to be done in background. The
	submit process on the commit server will schedule the transfers
	and complete the submit without waiting for transfers to finish.
	The archives on the originating edge server will be transferred to
	the commit server via pull threads initiated by submit on the
	commit server. Archives of ktext files will be transferred
	serially as usual rather than in background since archive content
	depends on commit server work. Also note that submitting pending
	(not shelved) changes using this feature will cause any
	change-submit triggers to be fired before edge-content triggers. An
	administrator must ensure that pull threads can be run on the commit
	server. The edge server IP address and port number used by the
	commit server to connect to the edge server must be specified in the
	ExternalAddress field of the edge server spec. The configurables
	submit.allowbgtransfer and lbr.autocompress must be set for all
	servers using this feature, including the commit server. This
	feature is not supported for graph depots. This option will be
	ignored if alternative file transfer or lbr.replication=shared is
	configured. This option is also ignored when submit is not run
	from an edge server, or when submit.allowbgtransfer is not set.
	If noretransfer is configured, noretransfer will be ignored.
	If archives failed to transfer to the commit server, they may be
	fetched on the commit server using 'p4 pull -u -t' to pull archives
	from the edge address specified by -t. The failed transfers are visible
	on the commit server via 'p4 pull -l' which also shows the target
	address for each failed transfer. Please see 'p4 help pull' for
	more details.

	Auto background archive transfer may be enabled by setting the
	submit.autobgtransfer configurable to 1 on the edge servers which
	require this functionality. When this is set in addition to the
	required settings for using -b, all submits from that edge will
	use background archive transfer without requiring -b on the command
	line.

	See 'p4 help-graph submit' for information on using this command with
	graph depots.
