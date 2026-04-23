# p4 files


    files -- List files in the depot

    p4 files [ -a ] [ -A ] [ -e ] [-i] [ -m max ] file[revRange] ...
    p4 files [ -a ] [ -e ] [ -m max ] [ --streamviews ] file[revRange] ...
    p4 files -U unloadfile ...

	List details about specified files: depot file name, revision,
	file, type, change action and changelist number of the current
	head revision. If client syntax is used to specify the file
	argument, the client view mapping is used to determine the
	corresponding depot files.

	By default, the head revision is listed.  If the file argument
	specifies a revision, then all files at that revision are listed.
	If the file argument specifies a revision range, the highest revision
	in the range is used for each file. For details about specifying
	revisions, see 'p4 help revisions'.

	The -a flag displays all revisions within the specific range, rather
	than just the highest revision in the range.

	The -A flag displays files in archive depots.

	The -e flag displays files with an action of anything other than
	deleted, purged or archived.  Typically this revision is always
	available to sync or integrate from.

	The -i flag is used to ignore the case of the file argument when
	listing files in a case sensitive server.

	The -m flag limits files to the first 'max' number of files.

	The -U option displays files in the unload depot (see 'p4 help unload'
	for more information about the unload depot).

	The --streamviews option reports results for streams in the context
	of the stream.

	See 'p4 help-graph files' for information on using this command with
	graph depots.
