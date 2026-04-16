# p4 switch


    switch -- Switch to a different stream, or create a new stream.

    p4 switch [-c -m -v -P parent] [-Rx] [-r] [ --no-sync ]
              [ --allow-unrelated ] stream
    p4 switch [-r -v] [-Rx] [ --no-sync ] [ --allow-unrelated ] [stream]@change
    p4 switch -l -L
    p4 switch

	The switch command switches to a different stream, optionally
	creating and populating that stream.

	The second form of the command is intended for reproducing
	back-in-time view and revision synchronization. To use this form,
	specify a change number and, optionally, a stream. The client will
	be switched to the stream specification that was available at the
	time of that change; also the workspace will be synced to that
	changelist.  If the stream is omitted but '@change' is specified,
	no stream switch is performed, but the workspace is synced to that
	change.

	The --allow-unrelated flag allows switching to a different stream
	hierarchy.

	The -c flag specifies that the new stream should be created and
	populated with a copy of all the files in the current stream.
	Local (dvcs) server only command.

	The -m flag specifies that 'switch -c' should create a new stream
	with no parent (an independent mainline). The new stream will be
	initially empty.
	Local (dvcs) server only command.

	The -P flag specifies that 'switch -c' should create a new stream
	with the specified stream as its parent, and thus the new stream
	will be populated with the files from the specified stream, rather
	than the files from the current stream.
	Local (dvcs) server only command.

	The -l flag specifies that all known streams should be listed.

	The -L flag specifies that all streams that were switched @change
	with files open in that stream should be listed.

	The -r flag reopens files in the new mapped location of the specified
	stream. If you omit the -r flag, and you have opened files in the
	current stream, those files are shelved before switching to the new
	stream, and are unshelved subsequently when you switch back to this
	stream.

	The -R option changes the way 'reconcile' is run when switching
	between streams:

	        -Ra     reconcile files not currently under Perforce control,
	                (these files are opened for add).
	        -Re     reconcile files that have been modified,
	                (these files are opened for edit).
	        -Rd     reconcile files that have been removed,
	                (these files are opened for delete).
	        -Rn     reconcile does not run.

	        Without this option, 'reconcile' runs as if all (-Raed)
	        options were set.

	The --no-sync flag prevents sync of file content for unopened files.

	The -v flag gives verbose information.

	With no arguments, the switch command displays the current stream.

	When switching to a different stream, the switch command first runs
	'p4 reconcile' to determine which files have been modified in the
	current stream.

	You cannot switch to a new stream if files are open in a numbered
	changelist. If files are open in the default changelist, they will
	be shelved and reverted prior to switching to the new stream, and
	will be automatically unshelved when switching back to this stream.

	After switching to the new stream, switch syncs your workspace to
	the head of the new stream, and unshelves any files which were open
	in the default changelist the last time you used that stream.

	See 'p4 help-graph switch' for information on using this command with
	graph depots.
