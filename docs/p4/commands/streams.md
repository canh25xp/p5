# p4 streams


    streams -- Display list of streams

    p4 streams [options] [-U] [streamPath ...]
    p4 streams [options] --viewmatch file[revRange] [streamPath ...]

	options: -a -F filter -m max -T fields

	Reports the list of all streams currently known to the system.  If
	a 'streamPath' argument is specified, the list of streams is limited
	to those matching the supplied path. Unloaded task streams are not
	listed by default.

	For each stream, a single line of output lists the stream depot path,
	the type, the parent stream depot path, and the stream name.

	The -a flag returns existing and deleted streams.

	The -F filter flag limits the output to files satisfying the expression
	given as 'filter'.  This filter expression is similar to the one used
	by 'jobs -e jobview', except that fields are case sensitive.  E.g.:

	    p4 streams -F "Parent=//Ace/MAIN&(Type=development|Type=release)"
	    p4 streams --viewmatch foo.c -F "PathType=import\&|PathType=share"

	Note: the filtering takes place post-compute phase; there are no
	indexes to optimize performance.

	The -m max flag limits output to the first 'max' number of streams.

	The -T fields flag (used with the -ztag global option) limits the
	fields output to those specified by a list given as 'fields'.
	These field names can be separated by a space or a comma.  E.g.:

	    p4 streams -T "Stream,Owner"

	The -U flag lists unloaded task streams (see 'p4 help unload').

	The --viewmatch flag returns streams whose views match the specified
	file(s).  The output includes the matching path entry for each stream.
	If a wildcard is specified, all overlapping path entries are returned.
