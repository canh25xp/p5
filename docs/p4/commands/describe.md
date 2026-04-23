# p4 describe


    describe -- Display a changelist description

    p4 describe [-d<flags> -a -m -s -S -f -O -I] changelist# ...

	Display a changelist description, including the changelist number,
	user, client, date of submission, textual description, list of
	affected files and diffs of files updated, affected stream spec and
	diff of updated stream spec.  Pending changelists are indicated as
	'pending' and file diffs are not displayed.

	For restricted changelists, 'no permission' is displayed if the user
	is not permitted to view the change (see 'p4 help change'). If a
	submitted change is restricted, the description is hidden unless
	the user is the owner of the change or has list permission for
	at least one file in the change. If a shelved change is restricted,
	the description is hidden unless the user is the owner of the change,
	or has list permission to at least one file in the change and has
	used the -S flag to request the shelved change. To view restricted
	pending (not shelved) changes, the user must be the owner of the
	change.

	The -a flag will display the content of added files in addition
	to diff of updated files.

	The -d<flags> passes one or more flags to the built-in diff routine
	to modify the output: -dn (RCS), -dc[n] (context), -ds (summary),
	-du[n] (unified), -db (ignore whitespace changes), -dw (ignore
	whitespace), -dl (ignore line endings). The optional argument to
	to -dc/-du specifies number of context lines.

	The -s flag omits the diffs of files that were updated.

	The -S flag lists a stream spec and files that are shelved for the
	specified changelist and displays diffs of the files against their
	previous revision.  If the change is restricted, the description is
	displayed according to the rules for shelved restricted changes
	described above.

	The -f flag forces display of the descriptions in a restricted
	change.  The -f flag requires 'admin' access, which is granted
	using 'p4 protect'.

	The -O flag specifies that the changelist number is the original
	number of a changelist which was renamed on submit.

	The -I flag specifies that the changelist number is the Identity
	field of a changelist.

	The -m flag limits files to the first 'max' number of files.

	See 'p4 help-graph describe' for information on using this command with
	graph depots.
