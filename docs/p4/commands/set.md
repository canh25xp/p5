# p4 set


    set -- Set or display Perforce variables

    p4 set [-q] [-s | -S service] [var[=[value]] ...]

	'p4 set' can set Perforce variables in persistent storage.

	On Windows, 'p4 set' sets Perforce variables in the Windows registry
	to the specified values.  On other platforms, 'p4 set' sets Perforce
	variables in the P4ENVIRO file.

	The 'p4 set' command can be used to set multiple variables in a
	single invocation, as well as to unset variables, or to display the
	current variable value. To display a variable's value, omit the
	'=value' and specify only the variable's name; to unset a variable,
	specify 'var=' but omit the value (as in: 'p4 set P4LOG=')

	With no arguments, 'p4 set' lists all variable settings, and list
	which persistent storage mechanism contained the variable's value.

	The -q flag removes the origins of the settings from the list output.

	The -s flag sets variables for the whole system rather than for the
	user.  This option requires NT administrator privilege.

	The -S service flag sets variables for the specified service. This
	option requires NT administrator privilege.

	Registry/P4ENVIRO variable entries can be overridden by environment
	variables and (in some cases) flags on the command line. See 'p4 help
	environment' for a list of environment variables used by Perforce.

	Certain Perforce variables can also be set persistently by using
	configuration files named by the P4CONFIG variable. These settings
	have higher precedence than environment/registry variables, but lower
	precedence than command line flags. P4CONFIG file variable settings
	only affect Perforce client programs; servers do not use them.

	Since Perforce variables can be set in multiple locations, you should
	examine the output of 'p4 set' carefully; a variable set in one
	location may be overriding a variable set in a lower-precedence
	location.
