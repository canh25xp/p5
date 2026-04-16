# p4 groups


    groups -- List groups (of users)

    p4 groups [-m max] [-v] [group]
    p4 groups [-m max] [-i [-v]] user | group
    p4 groups [-m max] [-g | -u | -o] name

	The first form lists all user groups defined in the server, or just
	the specified group.

	The second form displays subgroup relationships. If a user argument is
	specified, only groups containing that user are displayed. If a group
	argument is specified, only groups containing the group are displayed.

	The third form is useful when there are groups and users with the
	same name, or when requesting all groups owned by a certain user.

	The -i flag also displays groups that the user or group belongs to
	indirectly by means of membership in subgroups.

	The -m max flag limits output to the specified number of groups.

	The -v flag displays the MaxResults, MaxScanRows, MaxLockTime,
	MaxOpenFiles, MaxMemory, Timeout, and PasswordTimeout values
	for each group that is displayed.

	The -g flag indicates that the 'name' argument is a group.

	The -u flag indicates that the 'name' argument is a user.

	The -o flag indicates that the 'name' argument is an owner.
