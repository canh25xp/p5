# p4 ignores


    ignores -- List P4IGNORE mappings

    p4 ignores [-v] [path ...]
    p4 ignores [-v] -i path ...

	List the computed mappings from the rules found in P4IGNORE files
	at the specified path. The path(s) must be in local syntax. If no
	paths are specified, P4IGNORE rules from the current directory are
	reported.

	The -v flag displays additional information about the origin of the
	mapping. This includes the original rule, which file that was loaded
	from and the line number in that file.

	The -i flag allows you to test whether the path specified would be
	ignored by the rules in the P4IGNORE files. The target is listed if
	path matches one of the mappings. If the -v flag is also provided,
	the target will be reported with a message explaining if it will be
	ignored and if it matched a mapping, which P4IGNORE rule affected it.
