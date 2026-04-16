# p4 changes


    changes -- Display list of pending and submitted changelists
    changelists -- synonym for 'changes'

    p4 changes [-i -t -l -L -f -r] [-c client] [ -e changelist# ]
	    [-m max] [-s status] [-u user] [file[revRange] ...]
	    [--stream|--nostream]

	Returns a list of all pending and submitted changelists currently
	stored in the server.

	If files are specified, 'p4 changes' lists only submitted
	changelists that affect those files at the head revision.  If
	the file specification includes a revision range, 'p4 changes'
	lists only submitted changelists that affect the specified
	revisions.  See 'p4 help revisions' for details.

	If files are not specified, 'p4 changes' limits its report
	according to each change's type ('public' or 'restricted').
	If a submitted or shelved change is restricted, the change is
	not reported unless the user owns the change or has list
	permission for at least one file in the change. Only the owner
	of a restricted and pending (not shelved) change is permitted
	to see it.

	The -i flag also includes any changelists integrated into the
	specified files.

	The -t flag displays the time as well as the date.

	The -l flag displays the full text of the changelist
	descriptions.

	The -L flag displays the changelist descriptions, truncated to 250
	characters if longer.

	The -f flag enables admin users to view restricted changes.

	The -c client flag limits changes to those on the named client.

	The -e changelist# flag displays only changes that are above and
	including the specified changelist number.

	The -m max flag limits changes to the 'max' most recent.

	The -r flag sorts the output in reverse order.

	The -s status flag limits the output to changelists with the specified
	status. Specify '-s pending', '-s shelved', or '-s submitted'.

	The -u user flag displays only changes owned by the specified user.

	The --stream flag displays only changes that contain a stream spec.

	The --nostream flag displays only changes that do not contain a
	stream spec.

	For additional details on a single changelist, see 'p4 help describe'.
