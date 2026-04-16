# p4 fix


    fix -- Mark jobs as being fixed by the specified changelist

    p4 fix [-d] [-s status] -c changelist# jobName ...

	'p4 fix' marks each named job as being fixed by the changelist
	number specified with -c.  The changelist can be pending or
	submitted and the jobs can be open or closed (fixed by another
	changelist).

	If the changelist has already been submitted and the job is still
	open, then 'p4 fix' marks the job closed.  If the changelist has not
	been submitted and the job is still open, the job is closed when the
	changelist is submitted.  If the job is already closed, it remains
	closed.

	The -d flag deletes the specified fixes.  This operation does not
	otherwise affect the specified changelist or jobs.

	The -s flag uses the specified status instead of the default defined
	in the job specification.

	The fix's status is reported by 'p4 fixes', and is related to the
	job's status. Certain commands set the job's status to the fix's
	status for each job associated with the change. When a job is fixed
	by a submitted change, the job's status is set to match the fix
	status.  When a job is fixed by a pending change, the job's status
	is set to match the fix status when the change is submitted. If the
	fix's status is 'same', the job's status is left unchanged.
