# p4 resolved


    resolved -- Show files that have been resolved but not submitted

    p4 resolved [-o] [file ...]

	'p4 resolved' lists file updates and integrations that have been
	resolved but not yet submitted.  To see unresolved integrations,
	use 'p4 resolve -n'.  To see already submitted integrations, use
	'p4 integrated'.

	If a depot file path is specified, the output lists resolves for
	'theirs' files that match the specified path.  If a client file
	path is specified, the output lists resolves for 'yours' files
	that match the specified path.

	The -o flag reports the revision used as the base during the
	resolve.
