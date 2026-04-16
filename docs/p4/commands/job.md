# p4 job


    job -- Create or edit a job (defect) specification

    p4 job [-f] [jobName]
    p4 job -d jobName
    p4 job -o [jobName]
    p4 job -i [-f]

	The 'p4 job' command creates and edits job specifications using an
	ASCII form. A job is a defect, enhancement, or other unit of
	intended work.The 'p4 fix' command associates changelists with jobs.

	With no arguments, 'p4 job' creates an empty job specification
	and invokes the user's editor.  When the specification is saved,
	a job name of the form jobNNNNNN is assigned.  If the jobName
	parameter is specified on the command line, the job is created or
	opened for editing.

	As jobs are entered or updated, all fields are indexed for searching
	Text fields are broken into individual alphanumeric words (punctuation
	and whitespace are ignored) and each word is case-folded and entered
	into the word index.  Date fields are converted to an internal
	representation (seconds since 1970/01/01 00:00:00) and entered
	into the date index.

	The fields that compose a job are defined by the 'p4 jobspec' command.
	Perforce provides a default job specification that you can edit.

	The -d flag deletes the specified job. You cannot delete a job if
	it has pending or submitted fixes associated with it.

	The -o flag writes the job specification to the standard output.
	The user's editor is not invoked.

	The -i flag reads a job specification from the standard input. The
	user's editor is not invoked.

	The -f flag enables you set fields that are read-only by default.
	The -f flag requires 'admin' access, which is granted using the
	'p4 protect' command.
