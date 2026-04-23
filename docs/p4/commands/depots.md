# p4 depots


    depots -- Lists defined depots

    p4 depots [-t type] [[-e|-E] nameFilter]

	The depots command lists all depots defined in the server.

	The -t flag limits output to depots of the specified type. The
	following types are valid: 'local', 'spec', 'stream', 'remote',
	'archive', 'unload', 'graph', 'tangent', 'extension', 'trait'.

	The -e nameFilter flag lists depots with a name that matches
	the nameFilter pattern, for example: -e 'depo*'. The -e flag
	uses the server's normal case-sensitivity rules. The -E flag makes
	the matching case-insensitive, even on a case-sensitive server.
