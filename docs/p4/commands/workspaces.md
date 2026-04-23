# p4 workspaces


    clients -- Display list of clients
    workspaces -- synonym for 'clients'

    p4 clients [-t] [-u user] [[-e|-E] nameFilter] [-m max] [-S stream]
               [-a|-s serverID]
    p4 clients -U

	Lists all client workspaces currently defined in the server.

	The -t flag displays the time as well as the date.

	The -u user flag lists client workspaces that are owned by the
	specified user.

	The -e nameFilter flag lists workspaces with a name that matches
	the nameFilter pattern, for example: -e 'svr-dev-rel*'. The -e flag
	uses the server's normal case-sensitivity rules. The -E flag makes
	the matching case-insensitive, even on a case-sensitive server.

	The -m max flag limits output to the specified number of workspaces.

	The -S stream flag limits output to the client workspaces dedicated
	to the stream.

	The -U flag lists unloaded clients (see 'p4 help unload').

	The -a flag specifies that all clients should be displayed, not just
	those that are bound to this server.

	The -s serverID flag specifies that only those clients bound to the
	specified serverID should be displayed. On an Edge Server, the -s
	flag defaults to the Edge Server's serverID.
