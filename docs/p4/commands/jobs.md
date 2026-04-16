# p4 jobs


    jobs -- Display list of jobs

    p4 jobs [-e jobview -i -l -m max -r] [file[revRange] ...]
    p4 jobs -R

	Lists jobs in the server. If a file specification is included, fixes
	for submitted changelists affecting the specified files are listed.
	The file specification can include wildcards and a revision range.
	 See 'p4 help revisions' for details about specifying revisions.

	The -e flag lists jobs matching the expression specified in the
	jobview parameter. For a description of jobview syntax, see 'p4 help
	jobview'.

	The -i flag includes any fixes made by changelists integrated into
	the specified files.

	The -l flag produces long output with the full text of the job
	descriptions.

	The -m max flag limits the output to the first 'max' jobs, ordered
	by their job name.

	The -r flag sorts the jobs in reverse order (by job name).

	The -R flag rebuilds the jobs table and reindexes each job, which
	is necessary after upgrading to 98.2.  'p4 jobs -R' requires that the
	user be an operator or have 'super' access granted by 'p4 protect'.
