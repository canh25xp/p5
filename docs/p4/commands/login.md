# p4 login


    login -- Log in to Perforce by obtaining a session ticket

    p4 login [-s | -p] [-a | -h host] [user]
    p4 login [-s -a] -r <remotespec> [--remote-user=X]
    p4 login [-p -a] -r <remotespec> [--remote-user=X]

	The login command enables a user to access Perforce until the session
	expires or the user logs out.

	When a user logs in to Perforce, they are prompted for a password
	If they enter the correct password, they are issued a ticket.  The
	ticket expires when the default timeout value has been reached and
	is valid only for the host machine where the 'login' command was
	executed (see below for exception).

	The ticket can be used anywhere that a password can be used.

	Example: p4 -P <ticket value> changes -m1

	The -a flag causes the server to issue a ticket that is valid on all
	host machines. When used with -s, displays ticket status for all hosts.

	The -h flag causes the server to issue a ticket that is valid on the
	specified host (IP address).  This flag is typically used together
	with the -p flag to acquire a ticket that can be used on a different
	machine.

	The -p flag displays the ticket, but does not store it on the client
	machine.

	The -r flag causes the server to forward the login to the server
	referenced in the specified remote specification. The login will be for
	the user specified by the --remote-user flag, or if RemoteUser is set
	in the remote specification, the login will be for that user.
	Specifying a host or a username is not allowed when logging into a
	remote server.

	The -s flag displays the status of the current ticket (if there is
	one). Use with -a to display status for all hosts, or '-h host' to
	display status for a specfic host. Users with 'super' access can
	provide a username argument.

	Specifying a username as an argument to 'p4 login' requires 'super'
	access, which is granted by 'p4 protect'.  In this case, 'p4 login'
	does not prompt for the password (you must already be logged in).
