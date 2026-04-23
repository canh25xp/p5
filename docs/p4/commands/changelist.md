# p4 changelist


    change -- Create or edit a changelist description
    changelist -- synonym for 'change'

    p4 change [-s] [-f | -u] [[-O|-I] changelist#]
    p4 change -d [-f -s -O] changelist#
    p4 change -o [-s] [-f] [[-O|-I] changelist#]
    p4 change -i [-s] [-f | -u]
    p4 change -t restricted | public [-U user] [-f|-u|-O|-I] changelist#
    p4 change -U user [-t restricted | public] [-f] changelist#
    p4 change -d -f --serverid=X changelist#

	'p4 change' creates and edits changelists and their descriptions.
	With no argument, 'p4 change' creates a new changelist.  If a
	changelist number is specified, 'p4 change' edits an existing
	pending changelist.  In both cases, the changelist specification
	is placed into a form and the user's editor is invoked.

	The -d flag deletes a pending changelist, if it has no opened files
	(or stream spec) and no pending fixes associated with it.  Use
	'p4 opened -a' to report on opened files and 'p4 reopen' to move them
	to another changelist.  Use 'p4 fixes -c changelist#' to report on
	pending fixes and 'p4 fix -d -c changelist# jobs...' to delete pending
	fixes. The changelist can be deleted only by the user and client
	who created it, or by a user with 'admin' privilege using the -f
	flag.

	The -o flag writes the changelist specification to the standard
	output.  The user's editor is not invoked.

	The -i flag reads a changelist specification from the standard
	input.  The user's editor is not invoked.

	The -f flag forces operations on pending and submitted changelists
	and requires 'admin' access granted by 'p4 protect'. The -f and -u
	flags are mutually exclusive. By default, submitted changelists cannot
	be changed. The -f flag forces:

	    - Update or deletion of other users' pending changelists.
	    - Changing the 'User' of an empty pending change via -U.
	    - Deletion of submitted changelists after they have been
	      emptied of files using 'p4 obliterate' and of streams
	      using 'p4 stream --obliterate'.
	    - Update of the 'Description', 'Date', and 'User' fields of
	      submitted changelists.
	    - Display of the 'Description' field in a restricted changelist.

	The -u flag can force the update of a submitted change by the owner
	of the change. Only the Jobs, Type, and Description fields can be
	changed	using the -u flag. The -f and -u flags cannot be used in
	the same change command.

	The -s flag extends the list of jobs to include the fix status
	for each job.  On new changelists, the fix status begins as the
	special status 'ignore', which, if left unchanged simply excludes
	the job from those being fixed.  Otherwise, the fix status, like
	that applied with 'p4 fix -s', becomes the job's status when
	the changelist is committed.  Note that this option exists
	to support integration with external defect trackers.

	The -O flag specifies that the changelist number is the original
	number of a changelist which was renamed on submit.

	The -I flag specifies that the changelist number is the Identity
	field of a changelist.

	The -U flag changes the 'User' of an empty pending change to the
	specified user. The user field can only be changed using this flag
	by the user who created the change, or by a user with 'admin'
	privilege using the -f flag. This option is useful for running
	in a trigger or script.

	The -t flag changes the 'Type' of the change to 'restricted'
	or 'public' without displaying the change form. This option is
	useful for running in a trigger or script.

	The 'Type' field can be used to hide the change or its description
	from users. Valid values for this field are 'public' (default), and
	'restricted'. A shelved or committed change that is 'restricted' is
	accessible only to users who own the change or have 'list' permission
	to at least one file in the change.  A pending (not shelved)
	restricted change is only accessible to its owner.  Public changes
	are accessible to all users. This setting affects the output of the
	'p4 change', 'p4 changes', and 'p4 describe' commands. Note that
	the '-S' flag is required with 'p4 describe' for the command to
	enforce shelved	rather than pending restricted changelist rules.

	If a user is not permitted to have access to a restricted change,
	The 'Description' text is replaced with a 'no permission' message
	(see 'Type' field). Users with admin permission can override the
	restriction using the -f flag.

	In order to delete a pending changelist which is owned by a client
	on a different server, specify 'change -d -f --serverid=X', providing
	the serverid of the owning server. This variant of the change command
	must be issued directly to the Commit Server.
