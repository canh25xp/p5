# p4 edit


    edit -- Open an existing file or stream spec for edit

    p4 edit [-c changelist#] [-k -n] [-t filetype] [--remote=rmt] file ...
    p4 edit [-c changelist#] -So
    
	Open an existing file for edit.  The server records the fact that
	the current user has opened the file in the current workspace, and
	changes the file permission from read-only to read/write.

	If -c changelist# is included, the file opened in the specified
	pending changelist.  If changelist number is omitted, the file is
	opened in the 'default' changelist.

	If -t filetype is specified, the file is assigned that Perforce
	filetype. Otherwise, the filetype of the previous revision is reused.
	If a partial filetype is specified, it is combined with the current
	filetype.  For details, see 'p4 help filetypes'.
	Using a filetype of 'auto' will cause the filetype to be chosen
	as if the file were being added, that is the typemap will be
	considered and the file contents may be examined.

	The -n flag previews the operation without changing any files or
	metadata.

	The -k flag updates metadata without transferring files to the
	workspace. This option can be used to tell the server that files in
	a client workspace are already editable, even if they are not in the
	client view. Typically this flag is used to correct the Perforce
	server when it is wrong about the state of files in the client
	workspace, but incorrect use of this option can result in inaccurate
	file status information.

	The --remote flag is useful for DVCS configurations in which files
	of type +l are in use. 'p4 edit --remote=origin filename' opens the
	named file for edit in your DVCS server, and additionally, if the
	file is of type +l, takes a global exclusive lock on the origin
	server. That global exclusive lock will be retained until you push
	the updated file to the origin server, or until you use the
	'p4 revert --remote=origin filename' command to revert the file.

	The -So flag can be used with '-c change' to open the client's stream
	spec for edit.  (See 'p4 help streamcmds'.)

	See 'p4 help-graph edit' for information on using this command with
	graph depots.
