# p4 label


    label -- Create or edit a label specification

    p4 label [-f -g -t template] name
    p4 label -d [-f -g] name
    p4 label -o [-t template] name
    p4 label -i [-f -g]

	Create or edit a label. The name parameter is required. The
	specification form is put into a temporary file and the editor
	(configured by the environment variable $P4EDITOR) is invoked.

	The label specification form contains the following fields:

	Label:       The label name (read only.)

	Owner:       The user who created this label.  A label can be owned
	             by a user, or owned by a group.  Can be changed.

	Update:      The date that this specification was last modified.

	Access:      The date of the last 'labelsync' or use of '@label'
		     referencing this label.

	Description: A short description of the label (optional).

	Options:     Flags to change the label behavior.

	             locked       Prevents users other than the label owner
	             unlocked     from changing the specification. Prevents
	                          the label from being deleted. Prevents the
	                          owner from running 'p4 labelsync'. For a
	                          loaded label, prevents 'p4 unload'.

	             autoreload   For a static label, indicates where label
	             noautoreload revisions are stored. Specify 'noautoreload'
	                          to indicate that the revisions should be
	                          stored in the db.label table. Specify
	                          'autoreload' to indicate that the revisions
	                          should be stored in the unload depot.

	Revision:    An optional revision specification for an automatic
		     label.  Enclose in double quotes if it contains the
		     # (form comment) character.  An automatic label can
		     be treated as a pure alias of a single revision
		     specification (excluding @label) provided that the
		     View mapping is empty.

	View:        A mapping that selects files from the depot. The
		     default view selects all depot files. Only the left
		     side of the mapping is used for labels.  Leave this
		     field blank when creating an automatic label as
		     a pure alias. See 'p4 help views'.

	ServerID:    If set, restricts usage to the named server.
		     If unset, usage is allowed on any server.

	A label is a named collection of revisions. A label is either
	automatic or static. An automatic label refers to the revisions
	given in the View: and Revision: fields. When using automatic labels
	containing both View: and Revision: fields, use of the automatic labels
	to represent revision ranges might not produce the same results when
	using the equivalent changelist revision range. To make an automatic
	label behave exactly like its revision specifier, leave the View: field
	blank. Without this field, the automatic label is considered a pure
	alias and is processed exactly like the revision specification. A
	static label refers to the revisions that are associated with the label
	using the 'p4 tag' or 'p4 labelsync' commands. A static label cannot
	have a Revision: field. See 'p4 help revisions' for information on
	using labels as revision specifiers.

	Only the label owner can run 'p4 labelsync', and only if the label
	is unlocked. A label without an owner can be labelsync'd by any user.

	Flag -d deletes the specified label. You cannot delete a locked label.
	The -f flag forces the delete.

	The -o flag writes the label specification to standard output.  The
	user's editor is not invoked.

	The -i flag reads a label specification from standard input.  The
	user's editor is not invoked.

	The -t flag copies the view and options from the template label to
	the new label.

	The -f flag forces the deletion of a label. By default, locked labels
	can only be deleted by their owner.  The -f flag also permits the
	Last Modified date to be set.  The -f flag requires 'admin' access,
	which is granted by 'p4 protect'.

	The -g flag should be used on an Edge Server to update a global
	label. Without -g, the label definition is visible only to users
	of this Edge Server. Configuring rpl.labels.global=1 reverses this
	default and causes this flag to have the opposite meaning.
