# p4 reviews


    reviews -- List the users who are subscribed to review files

    p4 reviews [-C client] [-c changelist#] [file ...]

	'p4 reviews' lists all users who have subscribed to review the
	specified files.

	The -c flag limits the files to the submitted changelist.

	The -C flag limits the files to those opened in the specified clients
	workspace,  when used with the -c flag limits the workspace to files
	opened in the specified changelist.

	To subscribe to review files, issue the 'p4 user' command and edit
	the 'Reviews field'.
