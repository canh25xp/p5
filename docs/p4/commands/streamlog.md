# p4 streamlog


    streamlog -- List revision history of streams

    p4 streamlog [ -c  changelist# -h -i -l -L -t -m max ] stream ...

	List the revision history of the specified stream specs, from the most
	recent revision to the first.  If the stream was opened for edit
	and submitted, the change list information is displayed.
	Otherwise only the maximum change num at the time of edit is displayed.

	The -c flag displays the stream submitted at the specified
	changelist number.

	The -i flag includes inherited stream history. For a stream created by
	branching (using 'p4 integrate'), streamlog lists the revisions of the
	stream's ancestors up to the branch points that led to the specified
	revision.  Stream history inherited by renaming (using 'p4 move') is
	always displayed regardless of whether -i is specified.

	The -h flag displays the stream content history instead of stream name
	history.  The list includes revisions of other streams that were
	branched or copied (using 'p4 integrate' and 'p4 resolve -at') to
	the specified revision.  Revisions that were replaced by copying
	or branching are omitted, even if they are part of the history of
	the specified revision.

	The -t flag displays the time as well as the date.

	The -l flag lists the full text of the changelist descriptions.

	The -L flag lists the full text of the changelist descriptions,
	truncated to 250 characters if longer.

	The -m max displays at most 'max' revisions per stream
	argument specified.

	'p4 streamlog' reports on deleted streams.
