# p4 unlock


    unlock -- Release a locked file, leaving it open

    p4 unlock [-c | -s changelist# | -x] [-f] [file ...]
    p4 -c client unlock [-f] -r

	'p4 unlock' releases locks on the specified files.

	The changelist flag and file specification limit the files to be
	unlocked. If no file specification is given and no changelist is
	specified, all open files are unlocked.

	If a changelist is specified, only those files open in that
	changelist are unlocked.

	If a file specification is given, only those files are unlocked.

	If both changelist and file specification are provided, only the
	matching files in the specified changelist are unlocked.

	The -s flag unlocks files from a shelved changelist caused by an
	aborted 'submit -e' operation. The -c flag applies to opened files
	in a pending changelist locked by 'p4 lock' or by a failed submit
	of a change that is not shelved.

	By default, files can be unlocked only by the changelist owner who
	must also be the person who has the files locked. The -f flag
	enables you to unlock files in changelists owned by other users.
	The -f flag requires 'admin' access, which is granted by 'p4
	protect'.

	If 'p4 unlock' is called from an Edge Server, any corresponding
	files locked globally via 'p4 lock -g' by that client will be
	unlocked on the Commit Server.

	The -x option unlocks files that are opened 'exclusive' but are
	orphaned (see 'p4 opened -x'). This option only applies to a
	distributed installation where global tracking of these file types
	is necessary across servers. On the commit server, an administrator
	may specify 'p4 -c client unlock -f -x [file...]' to unlock the
	global exclusive locks of files which aren't marked orphaned.

	If a push command from a remote server to this server fails, files
	can be left locked on this server, preventing other users from
	submitting changes to those files. Files may also be left locked on
	a Commit Server from a failed submit or unlock. In these cases, the
	user who issued the failed command can use the second syntax form.
	The -r flag indicates the remote or commit server holding the lock.
	The -c global flag indicates the name of the client that issued the
	failed command. The -f flag allows the administrator to force the
	lock to be released.

	See 'p4 help-graph unlock' for information on using this command with
	graph depots.
