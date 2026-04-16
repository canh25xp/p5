# p4 user


    user -- Create or edit a user specification

    p4 user [-f] [name]
    p4 user -d [-f | -F] name
    p4 user -D [-f] [-y] name
    p4 user -o [name]
    p4 user -i [-f]

	Create, edit, or delete a user specification.
	The specification form is put into a temporary file and the editor
	(configured by the environment variable $P4EDITOR) is invoked.

	Normally, a user specification is created automatically the first
	time that the user issues any command that updates the depot. The
	'p4 user' command is typically used to edit the user's subscription
	list for change review.

	The user specification form contains the following fields:

	User:        The user name (read-only).

	Email:       The user's email address (Default: user@client).

	Update:      The date the specification was last modified (read-only).

	Access:      The date that the user last issued a client command.

	FullName:    The user's real name.

	JobView:     Selects jobs that are displayed when the user creates
		     a changelist. These jobs can be closed automatically
		     when the user submits the changelist. For a description
		     of jobview syntax, see 'p4 help jobview'

	Reviews:     The subscription list for change review.  There is no
	             limit on the number of lines that this field can contain.
		     You can include the following wildcards:

			 ...            matches any characters including /
			 *              matches any character except /

	Password:    The user's password.  See 'p4 help passwd'.

	Type:        Must be 'service', operator, or 'standard'. Default is
		     'standard'. Once set, the user type cannot be changed.

	AuthMethod:  Must be 'perforce' or 'ldap' and may include the '+2fa'
		     modifier.  The default is 'perforce' unless overridden
		     by the 'auth.default.method' configurable; see
		     'p4 help configurables'.  AuthMethod can only be changed
		      when the -f flag has been provided.

	The -d flag deletes the specified user (unless the user has files
	open).

	The -D flag deletes the specified user, the user's pending
	changelists, and these deleted changelists' associations to
	jobs and will revert the user's opened files. If the -F flag
	is also used, it will remove the user from the protections
	table, and remove the user from its groups. The -D flag will
	not revert files in this user's clients opened by other users,
	nor delete such clients, unless the -f or -F option is specified.
	Shelved files will not be affected. Use the -y flag to perform
	the operation. Without the -y flag, a preview of the actions
	is reported.

	The -o flag writes the user specification to the standard output.
	The user's editor is not invoked.

	The -i flag reads a user specification from the standard input.
	The user's editor is not invoked.

	The -f flag forces the creation, update or deletion of the specified
	user, and enables you to change the Last Modified date. By default,
	users can only delete or modify their own user specifications.  The
	-f flag requires 'super' access, which is granted by 'p4 protect'.

	The -F flag, used only with -d, forces the deletion of the
	specified user, and also removes the user from the protections
	table and from all groups. This command fails if removing the
	user from any group would cause the group to be deleted. In that
	case the group should be deleted prior to deleting the user.
	This flag can also be used with -D for the same purpose but
	requires -y for the action to be performed. The -F flag
	requires 'super' access, which is granted by 'p4 protect'.
