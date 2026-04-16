# p4 labels


    labels -- Display list of defined labels

    p4 labels [-U|-R] [-t] [-u user] [[-e|-E] nameFilter] [-m max]
	      [file[revrange]]
    p4 labels [-U|-R] [-t] [-u user] [[-e|-E] nameFilter] [-m max]
	      [-a|-s serverID]

	Lists labels defined in the server.

	If files are specified, 'p4 labels' lists the labels that contain
	those files.  If you include a file specification, automatic labels
	and labels with the 'autoreload' option set are omitted from the list.
	If the file specification includes a revision range, 'p4 labels'
	lists labels that contain the specified revisions.

	See 'p4 help revisions' for details about specifying revisions.

	The -t flag displays the time as well as the date.

	The -u user flag lists labels owned by the specified user.

	The -e nameFilter flag lists labels with a name that matches
	the nameFilter pattern, for example: -e 'svr-dev-rel*'. The -e flag
	uses the server's normal case-sensitivity rules. The -E flag makes
	the matching case-insensitive, even on a case-sensitive server.

	The -m max flag limits output to the first 'max' number of labels.

	The -U flag lists unloaded labels (see 'p4 help unload').
	
	The -R flag lists only labels with the 'autoreload' option set.

	The -a and -s flags are useful in a distributed server installation
	(see 'p4 help distributed') in order to see the names of local labels
	stored on other Edge Servers. These flags are not allowed if the
	command includes a file specification.

	The -a flag specifies that all labels should be displayed, not just
	those that are bound to this server.

	The -s serverID flag specifies that only those labels bound to the
	specified serverID should be displayed.

	On an Edge Server, if neither -s nor -a is specified, only those
	local labels bound to this Edge Server are displayed. Labels created
	on the Commit Server are global, and are also included in the output.
