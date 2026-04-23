# p4 labelsync


    labelsync -- Apply the label to the contents of the client workspace

    p4 labelsync [-a -d -g -n -q] -l label [file[revRange] ...]

	Labelsync causes the specified label to reflect the current contents
	of the client.  It records the revision of each file currently synced.
	The label's name can subsequently be used in a revision specification
	as @label to refer to the revision of a file as stored in the label.

	Without a file argument, labelsync causes the label to reflect the
	contents of the whole client, by adding, deleting, and updating the
	label.  If a file is specified, labelsync updates the specified file.

	If the file argument includes a revision specification, that revision
	is used instead of the revision synced by the client. If the specified
	revision is a deleted revision, the label includes that deleted
	revision.  See 'p4 help revisions' for details about specifying
	revisions.

	If the file argument includes a revision range specification,
	only files selected by the revision range are updated, and the
	highest revision in the range is used.

	If multiple arguments are given, they are applied to the label
	sequentially, such that the last revision specified for a given
	depot file is the one that will be recorded in the label.

	The -a flag adds the specified file to the label.

	The -d deletes the specified file from the label, regardless of
	revision.

	The -n flag previews the operation without altering the label.

	Only the owner of a label can run labelsync on that label. A label
	that has its Options: field set to 'locked' cannot be updated. A
	label without an owner can be labelsync'd by any user.

	The -q flag suppresses normal output messages. Messages regarding
	errors or exceptional conditions are displayed.

	The -g flag should be used on an Edge Server to update a global
	label. Note that in this case, the client should be a global client.
	Configuring rpl.labels.global=1 reverses this default and causes this
	flag to have the opposite meaning.
