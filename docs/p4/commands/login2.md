# p4 login2


    login2 -- Perform multi factor authentication

    p4 login2 [ -p -R ] [ -h host ] [ -S state ] [ -m method ] [ username ]
    p4 login2 -s [ -a | -h host ] [ username ]

	The login2 command enables a user requiring multi factor
	authentication to authorize access on a given host.

	The -p flag causes the multi factor authorization to persist even
	after the user's ticket has expired.  The authorization status can
	still be invalidated by running 'p4 logout'.

	The -R flag causes the multi factor authentication to be restarted
	allowing users to re-request a OTP, etc.

	The -s flag displays the multi factor authorization status for the
	user on the current host, or all hosts that the user has used if the
	-a flag is used.  To show the status for a specific host, the IP
	address  can be specified with the -h flag.

	Specifying a username as an argument to 'p4 login2' requires 'super'
	access, which is granted by 'p4 protect'.  In this case, 'p4 login2'
	skips the multi factor authentication process and immediately marks
	the user as validated for the current host (the super user must already
	be logged in and verified if necessary).  A host (IP address) can be
	specified with the -h flag to validate the user on a different host.

	For non-interactive clients, the -S flag can be used to execute each
	step of the multi factor authentication individually.  This must begin
	with the 'list-methods' state, which will report the list of available
	multi factor authentication methods for the given user.  The next
	state must be 'init-auth', and must be accompanied by the chose method
	provided to the -m flag: this will initiate the authentication with the
	multi factor authentication provider.  The final step is 'check-auth',
	which will either prompt for a OTP or request the authorization status
	from the multi factor authentication provider, depending on the type
	of authentication method selected.  The -p flag may be provided at the
	'init-auth' stage.  If a host and/or user is being specified, the
	appropriate arguments must be provided at each stage.
