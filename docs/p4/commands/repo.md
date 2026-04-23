# p4 repo


    repo -- Create, modify, or delete a repo specification

    p4 repo [-f] repo
    p4 repo -d [-f] repo
    p4 repo -o repo
    p4 repo -i [-f]

	A repo specification describes the high-level configuration
	and usage of a repo.

	The 'repo' command puts the repo spec into a temporary file and
	invokes the editor configured by the environment variable $P4EDITOR.
	Saving the file creates or modifies the repo spec.

	The repo spec contains the following fields:

	Name:        The name of the repo

	Owner:       The user who created this repo spec. Can be changed.

	Created:     The date this repo spec was created.

	Pushed:      The last time this repo was pushed to.

	Description: A short description of the repo (optional).

	MirroredFrom: Upstream URL this read-only repo is mirrored from.

	DefaultBranch: The branch to include when cloning the repo.
           It must start with "refs/" if you want git to use it; eg,
           "refs/heads/trunk" rather than just "trunk".

	The -d flag deletes the named repo spec and files it contains.

	The -o flag writes the repo spec to standard output. The user's
	editor is not invoked.

	The -i flag causes a repo spec to be read from the standard input.
	The user's editor is not invoked.

	The -f flag enables a user with 'admin' privilege to delete the repo.
	By default, repos can be deleted only by their owner. The owner of a
	repo or a user with 'super' privilege can update the 'Owner' field
	of a repo using -f.
