# p4 trust


	trust -- Establish trust of an SSL connection

	p4 trust [ -l -y -n -d -f -r -i <fingerprint> ]

	Establish trust of an SSL connection.  This client command manages
	the p4 trust file.  This file contains fingerprints of the keys
	received on ssl connections.  When an SSL connection is made, this
	file is examined to determine if the SSL connection has been used
	before and if the key is the same as a previously seen key for that
	connection.  Establishing trust with a connection prevents undetected
	communication interception (man-in-the-middle) attacks.

	Most options are mutually exclusive.  Only the -r and -f options
	can be combined with the others.

	The -l flag lists existing known fingerprints.

	Without options, this command will make a connection to a server
	and examine the key if present, if one cannot be found this command
	will show a fingerprint and ask if this connection should be trusted.
	If a fingerprint exists and does not match, an error that a possible
	security problems exists will be displayed.

	The -y flag will cause prompts to be automatically accepted.

	The -n flag will cause prompts to be automatically refused.

	The -d flag will remove an existing trusted fingerprint.

	The -f flag will force the replacement of a mismatched fingerprint.

	The -i flag will allow a specific fingerprint to be installed.

	The -r flag specifies that a replacement fingerprint is to be
	affected.  Replacement fingerprints can be used in anticipation
	of a server replacing its key.  If a replacement fingerprint
	exists for a connection and the primary fingerprint does not match
	while the replacement fingerprint does, the replacement fingerprint
	will replace the primary.  This flag can be combined with -l, -i,
	or -d.
