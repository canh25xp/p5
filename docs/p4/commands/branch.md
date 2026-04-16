# p4 branch


    branch -- Create, modify, or delete a branch view specification

    p4 branch [-f] name
    p4 branch -d [-f] name
    p4 branch [ -S stream ] [ -P parent ] -o name
    p4 branch -i [-f]

	A branch specification ('spec') is a named, user-defined mapping of
	depot files to depot files. It can be used with most of the commands
	that operate on two sets of files ('copy', 'merge', 'integrate',
	'diff2', etc.)

	Creating a branch spec does not branch files.  To branch files, use
	'p4 copy', with or without a branch spec.

	The 'branch' command puts the branch spec into a temporary file and
	invokes the editor configured by the environment variable $P4EDITOR.
	Saving the file creates or modifies the branch spec.

	The branch spec contains the following fields:

	Branch:      The branch spec name (read only).

	Owner:       The user who created this branch spec. Can be changed.

	Update:      The date this branch spec was last modified.

	Access:      The date of the last command used with this spec.

	Description: A description of the branch spec (optional).

	Options:     Flags to change the branch spec behavior. The defaults
		     are marked with *.

		locked   	Permits only the owner to change the spec.
		unlocked *	Prevents the branch spec from being deleted.

	View:        Lines mapping of one view of depot files to another.
		     Both the left and right-hand sides of the mappings refer
		     to the depot namespace.  See 'p4 help views' for more on
		     view syntax.

	New branch specs are created with a default view that maps all depot
	files to themselves.  This view must be changed before the branch
	spec can be saved.

	The -d flag deletes the named branch spec.

	The -o flag writes the branch spec to standard output. The user's
	editor is not invoked.

	The -i flag causes a branch spec to be read from the standard input.
	The user's editor is not invoked.

	The -f flag enables a user with 'admin' privilege to delete the spec
	or set the 'last modified' date.  By default, specs can be deleted
	only by their owner.

	A branch spec can also be used to expose the internally generated
	mapping of a stream to its parent. (See 'p4 help stream' and 'p4
	help streamintro'.)

	The -S stream flag will expose the internally generated mapping.
	The -P flag may be used with -S to treat the stream as if it were a
	child of a different parent. The -o flag is required with -S.
