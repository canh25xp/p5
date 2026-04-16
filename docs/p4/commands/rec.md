# p4 rec


    reconcile -- Open files for add, delete, and/or edit to reconcile
                 client with workspace changes made outside of Perforce

    rec         -- synonym for 'reconcile'
    status      -- 'reconcile -n + opened' (output uses local paths)
    status -A   -- synonym for 'reconcile -ead' (output uses local paths)

    clean       -- synonym for 'reconcile -w'

    p4 reconcile [-c change#] [-e -a -d -f -I -l -m -n -t] [-w [-K]] [file ...]
    p4 status [-c change#] [-A | [-e -a -d] | [-s]] [-f -I -m] [file ...]
    p4 clean [-e -a -d -I -l -K -m -n] [file ...]
    p4 reconcile -k [-l -n] [file ...]
    p4 status -k [file ...]

	'p4 reconcile' finds unopened files in a client's workspace and
	detects the following:

	1. files in depot missing from workspace, but still on have list
	2. files on workspace that are not in depot
	3. files modified in workspace that are not opened for edit

	By default, the files matching each condition above in the path
	are reconciled by opening files for delete (scenario 1), add
	(scenario 2), and/or edit (scenario 3). The -e, -a, and -d flags
	may be used to limit to a subset of these operations. If no file
	arguments are given, reconcile and status default to the current
	working directory.

	If the list of files to be opened includes both adds and deletes,
	the missing and added files will be compared and converted to pairs
	of move/delete and move/add operations if they are similar enough.

	In addition to opening unopened files, reconcile will detect files
	that are currently opened for edit but missing from the workspace
	and reopen them for delete. Reconcile will also detect files opened
	for delete that are present on the workspace and reopen them for
	edit.

	The -n flag previews the operation without performing any action.
	Although metadata updates from reconcile require open permission,
	the preview commands only require read access.

	If -c changelist# is included, the files are opened in the specified
	pending changelist.

	The -e flag allows the user to reconcile files that have been
	modified outside of Perforce. The reconcile command will open
	these files for edit.

	The -a flag allows the user to reconcile files that are in the
	user's directory that are not under Perforce source control. These
	files are opened for add.

	The -f flag allows the user to add files with filenames that contain
	wildcard characters. Filenames that contain the special characters
	'@', '#', '%' or '*' are reformatted to encode the characters using
	ASCII hexadecimal representation.  After the files are added, you
	must refer to them using the reformatted file name, because Perforce
	does not recognize the local filesystem name.

	The -I flag informs the client that it should not perform any ignore
	checking configured by P4IGNORE.

	The -d flag allows the user to reconcile files that have been
	removed from the user's directory but are still in the depot.
	These files will be opened for delete only if they are still on the
	user's have list.

	The -t flag allows the user to reconcile files that are in the
	user's directory that had their file type changed. The reconcile
	command will open these files for edit even if the content is
	unchanged. Reconcile detects and changes filetype when the base
	filetype or executable mode has been changed. Modifiers such as
	'+l' and '+k' will be preserved. The '+w' attribute is not
	detected or handled by -t. The '+x' modifier will be unchanged
	by -t on platforms that don't support executable file bits,
	like Windows and VMS.

	The -l flag requests output in local file syntax using relative
	paths, similar to the workspace-centric view provided by 'status'.

	The -m flag used in conjunction with -e can be used to minimize
	costly digest computation on the client by checking file modification
	times before checking digests to determine if files have been
	modified outside of Perforce.  If the modification times match the
	user's have list, the digest computations will be skipped.

	The -w flag forces the workspace files to be updated to match the
	depot rather than opening them so that the depot can be updated to
	match the workspace. The -a, -d, and -e flags when used with -w
	update workspace files as follows:

	  -a Files with no corresponding depot file are deleted.
	  -d Depot files not in the workspace are added.
	  -e Modified files are restored to the last version synced.

	Note the use of -w will result in the loss of any changes made
	to unopened files. Use of the -w flag requires read permission.

	The -k flag updates the have list when files in the workspace but
	not on the have list match content of corresponding files in the
	depot. In this case, the client's have list is updated to reflect
	the matching revisions. This option is used to reconcile the have
	list with the workspace.

	The -K flag (only used with 'p4 clean') suppresses keyword expansion
	when updating ktext files on the client.

	The -s flag (only used with 'p4 status') requests summarized
	output for the files to be opened for 'add'. Files in the current
	directory are listed as usual, but subdirectories containing files
	to be opened for 'add' are displayed instead of each file. This
	optimized option doesn't support move detection. Files to open
	for 'delete' and 'edit' are still listed individually.

	The status command displays preview output which includes files
	which are already opened in addition to the files that need to
	be reconciled. Opened files are not shown with options -A/-a/-e/-d.

	See 'p4 help-graph reconcile' for information on using this command
	with graph depots.
