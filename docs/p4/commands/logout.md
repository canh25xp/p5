# p4 logout


    logout -- Log out from Perforce by removing or invalidating a ticket.

    p4 logout [ -a | -h host ] [ -2 ] [ username ]

	The logout command removes the ticket on the client and invalidates the
	per-host ticket value on the server.  To resume using Perforce, the
	user must log in again.

	If you are logged in to Perforce from more than one machine, you can
	log out of Perforce from all machines from which you were logged in
	by specifying the -a flag.  The -a flag invalidates the ticket on the
	server.  All of your Perforce tickets are invalidated and you are
	logged out.  To invalidate the ticket for a specific host, you may
	specify the IP address of that host with the -h flag.

	The -2 flag causes only the user's multi factor authentication state
	to be invalidated.

	Note:
	  Only 'p4 logout -a' will invalidate tickets created by 'p4 login -a'.

	Specifying a username as an argument to 'p4 logout' requires 'super'
	access, which is granted by 'p4 protect'.  Note that either the '-a'
	or '-h' flags must be used to effectively log out a user who has a
	valid ticket on another machine.
