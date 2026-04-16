# p4 dirs


    dirs -- List depot subdirectories

    p4 dirs [-C -D -H] [-S stream] [-i] dir[revRange] ...
    p4 dirs [-D] [ --streamviews ] dir[revRange] ...

	List directories that match the specified file pattern (dir).
	This command does not support the recursive wildcard (...).
	Use the * wildcard instead.

	Perforce does not track directories individually. A path is treated
	as a directory if there are any undeleted files with that path as a
	prefix.

	By default, all directories containing files are listed. If the dir
	argument includes a revision range, only directories containing files
	in the range are listed. For details about specifying file revisions,
	see 'p4 help revisions'.

	The -i flag is used to ignore the case of the file pattern when
	listing directories in a case sensitive server. This flag is not
	compatible with the -C option.

	The -C flag lists only directories that fall within the current
	client view.

	The -D flag includes directories containing only deleted files.

	The -H flag lists directories containing files synced to the current
	client workspace.

	The -S flag limits output to depot directories mapped in a stream's
	client view.

	The --streamviews option reports results for streams in the context
	of the stream.

	See 'p4 help-graph dirs' for information on using this command with
	graph depots.
