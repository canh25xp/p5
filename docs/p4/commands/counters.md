# p4 counters


    counters -- Display list of known counters

    p4 counters [-e nameFilter -m max]

	Lists the counters in use by the server.  The server
	uses the following counters directly:

	    change           Current change number
	    job              Current job number
	    journal          Current journal number
	    lastCheckpointAction Data about the last complete checkpoint
	    logger           Event log index used by 'p4 logger'
	    maxCommitChange  Last change number successfully submitted
	    traits           Internal trait lot number used by 'p4 attribute'
	    upgrade          Server database upgrade level

	The -e nameFilter flag lists counters with a name that matches
	the nameFilter pattern, for example: -e 'mycounter-*'.

	The -m max flag limits the output to the first 'max' counters.

	The names 'minClient', 'minClientMessage', 'monitor',
	'security', 'masterGenNumber', and 'unicode' are
	reserved names: do not use them as ordinary counters.

	For general-purpose server configuration, see 'p4 help configure'.
