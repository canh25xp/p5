# p4 lock


    lock -- Lock an open file to prevent it from being submitted

    p4 lock [-c changelist#] [-g] [file ...]

	The specified files are locked in the depot, preventing any user
	other than the current user on the current client from submitting
	changes to the files.  If a file is already locked, the lock request
	is rejected.

	The -c flag and the file specification limit the files to be locked.
	If no file names are specified and no changelist is specified, all
	open files are locked.

	The -g flag is used to lock the files locally and globally at the
	Commit Server in a distributed environment. It may only be run from
	an Edge Server. When used to pre-lock files prior to submit, files
	should be opened in a numbered change in order to improve submit
	performance. These locks are removed by the 'unlock', 'revert',
	or any 'submit' command that operates on these files. Note that
	global locks will not be taken if any of the opened files specified
	in the command could not be opened locally.

	If a changelist is specified, only those files open in that
	changelist are locked.

	If a file specification is given, only the matching files are locked.

	If both the -c flag and a file specification are provided, only the
	matching files in the specified changelist are locked.

	See 'p4 help-graph lock' for information on using this command with
	graph depots.
