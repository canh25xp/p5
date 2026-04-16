# p4 delete


    delete -- Open an existing file for deletion from the depot

    p4 delete [-c changelist#] [-n -v -k] [--remote=rmt] file ...

	Opens a depot file for deletion.
	If the file is synced in the client workspace, it is removed.  If a
	pending changelist number is specified using with the -c flag, the
	file is opened for delete in that changelist. Otherwise, it is opened
	in the default pending changelist.

	Files that are deleted generally do not appear on the have list.

	The -n flag displays a preview of the operation without changing any
	files or metadata.

	The -k flag performs the delete on the server without modifying
	client files.  Use with caution, as an incorrect delete can cause
	discrepancies between the state of the client and the corresponding
	server metadata.

	The -v flag enables you to delete files that are not synced to the
	client workspace. The files should be specified using depot syntax;
	since the files are not synced, client syntax or local syntax are
	inappropriate and could introduce ambiguities in the file list.
	Note, though, that if the files ARE synced, 'delete -v' will remove
	the files from the client in addition to opening them for delete.
	The preferred way to delete a set of files without transferring
	them to your machine is: 'sync -k file...', then 'delete -k file...'

	The --remote flag is useful for DVCS configurations in which files
	of type +l are in use. 'p4 delete --remote=origin filename' opens the
	named file for deletion in your DVCS server, and additionally, if the
	file is of type +l, takes a global exclusive lock on the origin
	server. That global exclusive lock will be retained until you push
	the deleted file to the origin server, or until you use the
	'p4 revert --remote=origin filename' command to revert the file.

	See 'p4 help-graph delete' for information on using this command with
	graph depots.
