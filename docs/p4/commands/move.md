# p4 move


    move -- move file(s) from one location to another
    rename -- synonym for 'move'

    p4 move [-c changelist#] [-f -n -k] [-t filetype] fromFile toFile
    p4 move -r [-c changelist#] [-n -k] fromFile toFile

	Move by default takes an already opened file and moves it from one
	client location to another, reopening it as a pending depot move.
	When the file is submitted with 'p4 submit', its depot file is moved
	accordingly. The 'p4 move' command is intended for moves that occur
	within the same branch and not for restructuring the depot by moving
	an entire branch.

	Wildcards in fromFile and toFile must match. The fromFile must be
	a file open for add or edit unless invoked with '-r'.

	'p4 opened' lists pending moves. 'p4 diff' can compare a moved
	client file with its depot original, 'p4 sync' can schedule an
	update of a moved file, and 'p4 resolve' can resolve the update.

	A client file can be moved many times before it is submitted.
	Moving a file back to its original location will undo a pending
	move, leaving unsubmitted content intact.  Using 'p4 revert'
	undoes the move and reverts the unsubmitted content.

	If -c changelist# is specified, the file is reopened in the
	specified pending changelist as well as being moved.

	The -f flag forces a move to an existing target file. The file
	must be synced and not opened.  The originating source file will
	no longer be synced to the client.

	If -t filetype is specified, the file is assigned that filetype.
	If the filetype is a partial filetype, the partial filetype is
	combined with the current filetype.  See 'p4 help filetypes'.

	The -n flag previews the operation without moving files.

	The -k flag performs the rename on the server without modifying
	client files. Use with caution, as an incorrect move can cause
	discrepancies between the state of the client and the corresponding
	server metadata.

	The -r flag may be used to rename existing files without altering
	content or type to a new target. This option allows the user to
	move files from the depot to a different location without opening
	the files first. Since this option performs a strict rename only,
	the '-t' and '-f' flags are not allowed, source files must not be
	already opened, and the targets must not already exist.

	The 'move' command requires a release 2009.1 or newer client. The
	'-f' flag requires a 2010.1 client.
