# p4 group


    group -- Change members of user group

    p4 group [-a|-A] name
    p4 group -d [-a | -F] name
    p4 group -o name
    p4 group -i [-a|-A]

	Create a group or modify the membership of an existing group.
	A group can contain users and other groups. The group specification
	is put into a temporary file and the editor (configured by the
	environment variable $P4EDITOR) is invoked.

	A group exists when it has any users or other groups in it, and
	ceases to exist if all users and groups in it are removed.

	Each group has MaxResults, MaxScanRows, MaxLockTime, MaxOpenFiles
	and MaxMemory fields, which limit the resources committed to
	operations performed by members of the group.  See
	'p4 help maxresults' for more information on MaxResults, MaxScanRows,
	MaxLockTime, MaxOpenFiles and MaxMemory.

	Since a user may be a member of multiple groups, the effective
	resource limits for that user take into account the resource limits
	specified by each group to which that user belongs, except for those
	groups where the limit is specified as 'unset'.

	After examining all relevant groups, an individual user's limit is
	the highest of any group with a limit to which he belongs, unlimited
	if any of his groups has 'unlimited' for that field, or unlimited
	if he belongs to no group with a limit.

	Each group also has a Timeout field, which specifies how long (in
	seconds) a 'p4 login' ticket remains valid.  A value of 'unset' or
	'unlimited' is equivalent to no timeout. An individual's timeout is
	the highest of any group with a limit to which he belongs, unlimited
	if any of his groups has 'unlimited' for the timeout value, or
	unlimited if he belongs to no group with a limit. See 'p4 help login'
	for more information.

	Each group has a PasswordTimeout field, which determines how long a
	password remains valid for members of the group.

	A group may be synchronized with an LDAP group by setting the three
	fields: LdapConfig, LdapSearchQuery and LdapUserAttribute. This takes
	the LDAP configuration (see 'p4 ldap') specified by LdapConfig and uses
	it to execute the query stored by LdapSearchQuery. The LDAP attribute
	specified by LdapUserAttribute is taken to be user's username and is
	added to the group's user list.  If the group contains references its
	members by their DNs rather than by username, you may instruct the DN
	to be followed by setting the LdapUserDNAttribute field to the name of
	the group object's attribute that the DNs are stored in: the username
	will be found by looking for the LdapUserAttribute attribute in the
	object referred to by the DN.  At least one group owner must be set
	if these LDAP fields are used.  If the LDAP server requires login for
	read-only queries, then the LDAP configuration must contain valid bind
	credentials in the LDAP spec's SearchBindDN and SearchPasswd fields

	The -d flag deletes a group.

	The -o flag writes the group specification to standard output. The
	user's editor is not invoked.

	The -i flag reads a group specification from standard input. The
	user's editor is not invoked.  The new group specification replaces
	the previous one.

	The -a flag enables a user without 'super' access to modify the group
	if that user is an 'owner' of that group. Group owners	are specified
	in the 'Owners' field of the group spec.

	The -A flag enables a user with 'admin' access to add a new group.
	Existing groups may not be modified when this flag is used.

	The -F flag, used only with -d, forces the deletion of the
	specified group, and also removes the group from the protections
	table and from all groups.

	All commands that require access granted by 'p4 protect' consider a
	user's groups when calculating access levels.

	'p4 group' requires 'super' access granted by 'p4 protect' unless
	invoked with the '-a' or '-A' flag by a qualified user.
