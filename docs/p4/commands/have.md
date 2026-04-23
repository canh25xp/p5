# p4 have


    have -- List the revisions most recently synced to the current workspace

    p4 have [--graph-only] [file ...]

	List revision numbers of the currently-synced files. If file name is
	omitted, list all files synced to this client workspace.

	The format is:  depot-file#revision - client-file

	For hybrid workspaces that have both classic and graph depots, the
	--graph-only flag limits the report to graph depots only. Without
	it 'p4 have' will report on both graph and classic depots.

	See 'p4 help-graph have' for information on using this command with
	graph depots.
