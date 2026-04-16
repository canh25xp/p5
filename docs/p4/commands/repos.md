# p4 repos


    repos -- Display list of repo specifications

    p4 repos [[-e|-E] nameFilter] [-m max] [-u user] [-O owner]
             [--from=origin]

	Lists repo specifications. (See 'p4 help repo'.)

	The -e nameFilter flag lists repo specs with a name that matches
	the nameFilter pattern, for example: -e '//linux/kernel/...'.

	The -e flag uses the server's normal case-sensitivity rules.

	The -E flag makes the matching case-insensitive, even on a
	case-sensitive server.

	The -m max flag limits output to the specified number of repo specs.

	The -O owner flag limits output to repos owned by the specified user
	or group.

	The -u user flag limits output to those repos that can be read by the
	specified user.

	The --from=origin flag limits output to repos which were forked from
	the specified repo.
