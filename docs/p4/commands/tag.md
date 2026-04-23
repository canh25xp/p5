# p4 tag


    tag -- Tag files with a label

    p4 tag [-d -g -n -U] -l label file[revRange] ...

	Tag associates the named label with the file revisions specified by
	the file argument.  After file revisions are tagged with a label,
	revision specifications of the form '@label' can be used to refer
	to them.

	If the file argument does not include a revision specification, the
	head revisions is tagged.  See 'p4 help revisions' for revision
	specification options.

	If the file argument includes a revision range specification, only
	the files with revisions in that range are tagged.  Files with more
	than one revision in the range are tagged at the highest revision.

	If multiple arguments are given, they are applied to the label
	sequentially, such that the last revision specified for a given
	depot file is the one that will be tagged with the label.

	The -d deletes the association between the specified files and the
	label, regardless of revision.

	The -n flag previews the results of the operation.

	Tag can be used with an existing label (see 'p4 help labels') or
	with a new one.  An existing label can be used only by its owner,
	and only if it is unlocked. (See 'p4 help label').

	The -U flag specifies that the new label should be created with the
	'autoreload' option (See 'p4 help label'). It has no effect on an
	existing label.

	To list the file revisions tagged with a label, use 'p4 files
	@label'.

	The -g flag is used on an Edge Server to update a global label.
	Configuring rpl.labels.global=1 reverses this default and causes this
	flag to have the opposite meaning.

	See 'p4 help-graph tag' for information on using this command with
	graph depots.
