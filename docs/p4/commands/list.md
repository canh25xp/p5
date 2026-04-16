# p4 list


    list -- Create a temporary list of files that can be used as a label

    p4 list [ -l label ] [ -C ] [ -M ] file[revRange] ...
    p4 list -l label -d [ -M ]

	List builds an in-memory temporary list of files that can be used
	as a label in any command that can take a label. The list only lasts
	as long as the connected session, for example a temporary list created
	in one 'p4' command line request will not be available to a subsequent
	'p4' command that references the label.

	By default, the head revision is listed.  If the file argument
	specifies a revision, then all files at that revision are listed.
	If the file argument specifies a revision range, the highest revision
	in the range is used for each file. For details about specifying
	revisions, see 'p4 help revisions'.

	The -l flag specifies the name of the temporary label that should be
	used to access the list.  In the event that a label with that name
	already exists, the in-memory name has precedence over the stored one.
	Without the '-l' option the list command assigns a unique name to the
	temporary list and returns the name as output.

	The -d flag specifies that the named list should be deleted, freeing
	the memory used by that list.

	The -C flag limits any depot paths to those that can be mapped through
	the client workspace.

	The -M flag can be specified when issuing the list command against a
	forwarding replica; in this case, it indicates that the list command
	should not be processed by the replica, but should instead be
	forwarded to the master server.
