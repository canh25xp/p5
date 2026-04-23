# p4 istat


    istat -- Show/cache a stream's integration status

    p4 istat [-As | -Af] [-a | -r] [-c | -C] [ -s ] stream

	'p4 istat' shows a stream's cached integration status with respect
	to its parent. If the cache is stale, either because newer changes
	have been submitted or the stream's branch view has changed, 'p4
	istat' checks for pending integrations and updates the cache before
	showing status.

	Pending integrations are shown only if they are expected by the
	stream; that is, only if they are warranted by the stream's type
	and its fromParent/toParent flow options. (See 'p4 help stream'.)

	The -r flag shows the status of integration to the stream from its
	parent. By default, status of integration in the other direction is
	shown, from the stream to its parent.

	The -a flag shows status of integration in both directions.

	The -c flag forces 'p4 istat' to assume the cache is stale; it
	causes a search for pending integrations.  Use of this flag can
	impact server performance. The -C flag performs the same actions,
	and additionally clears the stream's record of the highest merged
	changelist.

	The -s flag shows cached state without refreshing stale data.

	'p4 istat' will report the combined integration status for both stream
	files and the stream spec of the specified stream.

	Use -Af or -As flag to limit the report of the status of files only or
	the stream spec only:

		-Af	Report integration status for stream files only.
		-As	Report integration status for the stream spec only.

	If the cache is cleared, the integration status for both files and
	stream spec are discovered, even if the requested report is only for
	files or only for the stream spec.
	
	See 'p4 help streamspecintg' for more details.
