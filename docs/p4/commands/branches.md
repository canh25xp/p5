# p4 branches


    branches -- Display list of branch specifications

    p4 branches [-t] [-u user] [[-e|-E] nameFilter] [-m max]

	Lists branch specifications. (See 'p4 help branch'.)

	The -t flag displays the time as well as the date.

	The -u user flag lists branch specs owned by the specified user.

	The -e nameFilter flag lists branch specs with a name that matches
	the nameFilter pattern, for example: -e 'svr-dev-rel*'. The -e flag
	uses the server's normal case-sensitivity rules. The -E flag makes
	the matching case-insensitive, even on a case-sensitive server.

	The -m max flag limits output to the specified number of branch specs.
