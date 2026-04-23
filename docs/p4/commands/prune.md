# p4 prune


    prune -- Remove unmodified branched files from a stream

    p4 prune [-d] [-y] -S stream

	Prune permanently removes unmodified files from a stream that is
	no longer being actively used.  Only the owner of a stream may
	'p4 prune' it.

	After a stream has been pruned, files that have been modified, i.e.
	files with more than one revision, will remain in the stream so that
	their edit history will be preserved.  The unmodified files will be
	gone as if obliterated by an administrator (see 'p4 help obliterate').

	Mainline, task, and virtual streams may not be pruned.  To remove
	unmodified files from a task stream, delete or unload it (see 'p4 help
	stream' and 'p4 help unload').

	Pruned files will remain in client workspaces until the next 'p4 sync',
	which will remove them.  If pruned files have been branched to a child
	stream, new integration records will be generated to directly link the
	branched files in the child stream to the files in the parent stream
	that they were previously related to indirectly.

	If the -d flag is given, a deep prune is performed which also removes
	files whose only changes in the target stream were due to merges.

	By default, prune displays a preview of the results.  To execute the
	operation, you must specify the -y flag.

	'p4 prune' requires 'write' access, which is granted by 'p4 protect'.
	You must also be the owner of the stream in order to run 'p4 prune'.
