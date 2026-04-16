# p4 reopen


    reopen -- Change the filetype of an open file or move it to
              another changelist (or move an opened stream spec)

    p4 reopen [-c changelist#] [-t filetype | -Si] file ...
    p4 reopen -c changelist# -So

	Reopen an open file for the current user in order to move it to a
	different changelist or change its filetype.

	The target changelist must exist; you cannot create a changelist by
	reopening a file. To move a file to the default changelist, use
	'p4 reopen -c default'.

	The '-c' flag with file arguments will not allow a move pair to
	be split. The command will fail if only the 'movedFrom' file is
	specified. The command can succeed when only the 'movedTo' file
	is specified.

	If -t filetype is specified, the file is assigned that filetype. If
	a partial filetype is specified, it is combined with the current
	filetype.  For details, see 'p4 help filetypes'.

	The -So flag can be used with '-c change' to move the client's open
	stream spec to a different changelist.  The -Si flag can be used with
	a file list to include an open stream spec when also reopening files.
	For example, to move an open stream spec to the default changelist,
	use 'p4 reopen -c default -So'.  (See 'p4 help streamcmds'.)
