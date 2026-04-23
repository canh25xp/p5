# p4 users


    users -- List Perforce users

    p4 users [-l -a -r -c] [-m max] [user ...]

	Lists all Perforce users or users that match the 'user' argument.
	The report includes the last time that each user accessed the system.

	The -m max flag limits output to the first 'max' number of users.

	The -a flag includes service and operator users in the output.

	The -l flag includes additional information in the output.  The -l
	flag requires 'super' access, which is granted by 'p4 protect'.

	The -r and -c flags are only allowed on replica servers.  When
	-r is given only users who have used a replica are reported and
	when -c is given only the user information from the central server
	is reported.  Otherwise on a replica server, the user list will
	be slightly different from the master server as the user access times
	will reflect replica usage or master usage whichever is newer.
