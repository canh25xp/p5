# p4 fixes


    fixes -- List jobs with fixes and the changelists that fix them

    p4 fixes [-i -m max -c changelist# -j jobName] [file[revRange] ...]

	'p4 fixes' list fixed jobs and the number of the changelist that
	 contains the fix. Fixes are associated with changelists using the
	'p4 fix' command or by editing and submitting changelists.

	The 'p4 fixes' command lists both submitted and pending changelists.

	By default, 'p4 fixes' lists all fixes.  This list can be limited
	as follows: to list fixes for a specified job, use the -j jobName
	flag.  To list fixes for a specified changelist, use -c changelist#.
	To list fixes that affect specified files, include the file argument.
	The file pattern can include wildcards and revision specifiers. For
	details about revision specifiers, see 'p4 help revisions'

	The -i flag also includes any fixes made by changelists integrated
	into the specified files.

	The -m max flag limits output to the specified number of job
	fixes.
