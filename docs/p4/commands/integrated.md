# p4 integrated


    integrated -- List integrations that have been submitted

    p4 integrated [-b branch [-r]] [-s change | -m max ]
	          [--into-only] [file ...]

        The p4 integrated command lists integrations that have been submitted.
        To list unresolved integrations, use 'p4 resolve -n'.  To list
        resolved but unsubmitted integrations, use 'p4 resolved'.

        If the -b branch flag is specified, only files integrated from the
        source to target files in the branch view are listed.  Qualified
        files are listed, even if they were integrated without using the
        branch view.

        The -r flag reverses the mappings in the branch view, swapping the
        target files and source files.  The -b branch flag is required.

        The -s flag takes a change number and shows integrations from that
        change forward. This option cannot be used with -m.

        The -m flag limits integrations to the 'max' most recent. The
        output is sorted by descending change number. This option cannot
        be used with -s.

        The --into-only flag shows only integrations from this path into
        other paths, not integrations from other paths into this one,
        or changes ignored by the target path.
