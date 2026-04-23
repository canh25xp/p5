# p4 opened


    opened -- List open files or stream spec and display status.

    p4 opened [-a -c changelist# -C client -u user -m max -s -g] [file ...]
    p4 opened [-a -x -m max ] [file ...]
    p4 opened [-c changelist#] -So

	Lists files currently opened in pending changelists, or, for
	specified files, show whether they are currently opened or locked.
	If the file specification is omitted, all files open in the current
	client workspace are listed.

	Files in shelved changelists are not displayed by this command. To
	display shelved changelists, see 'p4 changes -s shelved'; to display
	the files in those shelved changelists, see 'p4 describe -s -S'.

	If the stream spec associated with this client is currently opened,
	the stream spec is displayed on the first line of the output.

	By default, files opened by the current user in the current client
	client workspace are listed. The -a, -C client, -u user, and
	-c changelist# flags alter this behavior. The -a flag lists opened
	files in all clients. The -C client flag lists files open in the
	specified client workspace. The -u user flag lists files opened by
	the specified user. The -C and -u flags may be combined; -a is
	ignored if either of the -C or -u flags is used. The -C, -u, and -a
	flags are ignored if the -c flag is used.

	The -c changelist# flag lists files opened in the specified
	changelist#.

	The -m max flag limits output to the first 'max' number of files.

	The -s option produces 'short' and optimized output when used with
	the -a (all clients) option.  For large repositories '-a' can take
	a long time when compared to '-as'.

	The -x option lists files that are opened 'exclusive'. This option
	only applies to a distributed installation where global tracking of
	these file types is necessary across servers. The -x option implies
	the -a option.

	The -g option lists files that are opened on the Commit Server
	in a distributed installation. This is useful for tracking files
	that may be globally locked on the Commit Server from any of its
	Edge Servers. Since the command query runs on the Commit Server,
	the file spec argument (if any) must be in depot syntax. The -g
	option implies the -a option.

	The -So flag can be used with '-c change' to list the stream spec
	which is currently opened for edit. By default, if -So is omitted,
	the opened stream spec will be listed before the list of files.
	If no stream spec is opened for edit, a stream spec will not be
	listed.  (See 'p4 help streamcmds'.)

	See 'p4 help-graph opened' for information on using this command with
	graph depots.
