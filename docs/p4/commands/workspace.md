# p4 workspace


    client -- Create or edit a client workspace specification and its view
    workspace -- Synonym for 'client'

    p4 client [-f] [-t template] [-T type] [name]
    p4 client -d [-f [-Fs]] name
    p4 client -o [-t template] [-T type] [name]
    p4 client -S stream [[-c change] -o] [name]
    p4 client -s [-f] -S stream [name]
    p4 client -s [-f] -t template [name]
    p4 client -i [-f]
    p4 client -d -f --serverid=X [-Fs] name

	Creates a new client specification ('spec') or edits an existing
	spec.  A client spec is a named mapping of depot files to workspace
	files.

	The 'p4 client' command puts the client spec into a temporary file
	and invokes the editor configured by the environment variable
	$P4EDITOR.  For new workspaces, the client name defaults to the
	$P4CLIENT environment variable, if set, or to the current host name.
	Saving the file creates or modifies the client spec.

	The client spec contains the following fields:

	Client:      The client name.

	Host:        If set, restricts access to the named host.
		     If unset, access is allowed from any host.

	Owner:       The user who created this client.

	Update:      The date that this spec was last modified.

	Access:      The date that this client was last used in any way.

	Description: A short description of the workspace.

	Root:        The root directory of the workspace (specified in local
		     file system syntax), under which all versioned files
		     will be placed. If you change this setting, you must
		     physically relocate any files that currently reside
		     there.  On Windows client machines, you can specify the
		     root as "null" to enable you to map files to multiple
		     drives.

	AltRoots:    Up to two optional alternate client workspace roots.
		     The first of the main and alternate roots to match the
		     client program's current working directory is used. If
		     none match, the main root is used. 'p4 info' displays
		     the root that is being used.

	Options:     Flags to configure the client behavior. Defaults
		     are marked with *.

		allwrite	Leaves all files writable on the client;
		noallwrite *	by default, only files opened by 'p4 edit'
				are writable. If set, files might be clobbered
				as a result of ignoring the clobber option
				(see below).

		clobber		Permits 'p4 sync' to overwrite writable
		noclobber *	files on the client.  noclobber is ignored if
				allwrite is set.

		compress 	Compresses data sent between the client
		nocompress *	and server to speed up slow connections.

		locked   	Allows only the client owner to use or change
		unlocked *	the client spec.  Prevents the client spec from
				being deleted or unloaded.

		modtime  	Causes 'p4 sync' and 'p4 submit' to preserve
		nomodtime *	file modification time, as with files with the
				+m type modifier. (See 'p4 help filetypes'.)
				With nomodtime, file timestamps are updated by
				sync and submit operations.

		rmdir		Makes 'p4 sync' attempt to delete a workspace
		normdir *	directory when all files in it are removed.

		altsync		Instructs the client to use an alternative sync
		noaltsync *	agent specified with P4ALTSYNC when performing
				file operations in the workspace. This option
				can only be changed when the client's have list
				is empty.

	SubmitOptions:  Flags to change submit behavior.

		submitunchanged     All open files are submitted (default).

		revertunchanged     Files that have content, type, or resolved
		                    changes are submitted. Unchanged files are
		                    reverted.

		leaveunchanged      Files that have content, type, or resolved
		                    changes are submitted. Unchanged files are
		                    moved to the default changelist.

		        +reopen     Can be appended to the submit option flag
		                    to cause submitted files to be reopened in
		                    the default changelist.
		                    Example: submitunchanged+reopen

	LineEnd:    Set line-ending character(s) for client text files.

		local		mode that is native to the client (default).
		unix		linefeed: UNIX style.
		mac		carriage return: Macintosh style.
		win		carriage return-linefeed: Windows style.
		share		hybrid: writes UNIX style but reads UNIX,
				Mac or Windows style.

	View:        Maps files in the depot to files in your client
		     workspace.  Defines the files that you want in your
		     client workspace and specifies where you want them
		     to reside.  The default view maps all depot files
		     onto the client.  See 'p4 help views' for view syntax.
		     A new view takes effect on the next 'p4 sync'.

	ChangeView:  Restricts depot paths to a particular point in time.
	             Example: //depot/path/...@1000
	             Revisions submitted to the path after the specified
	             changelist will not be visible.  Files matching a
	             ChangeView path may not be submitted.

	             Automatic labels may also used to specify a particular
	             point in time: the label's 'Revision' field must be set
	             to a changelist number or a date.  If the label has a
	             view, only files that match the intersection of the
	             ChangeViewpath and the label's view will be restricted to
	             the point in time specified by the 'Revision' field; any
	             other files matching the ChangeView path may sync to
	             #head, but cannot be submitted.

	Stream:      The stream to which this client's view will be dedicated.
		     (Files in stream paths can be submitted only by dedicated
		     stream clients.) When this optional field is set, the
		     View field will be automatically replaced by a stream
		     view as the client spec is saved.

	StreamAtChange:
		     Stream meta-data (for view creation) is versioned.
		     When StreamAtChange is set, the stream hierarchy is
		     constructed back-in-time to reproduce the view that was
		     available to clients in the past. With this field set
		     changes cannot be be submitted from the client.
		     StreamAtChange is optional; Stream must be set as well.

	ServerID:    If set, restricts usage to the named server.
		     If unset, usage is allowed on the master server and on
		     any replicas of the master other than Edge Servers.

	Type:        By default all clients are 'writeable', certain clients
		     are short lived and perform long sync and build cycles.
		     Over time these build clients can fragment the 'db.have'
		     table which is used to track what files a client has
		     synced.  Setting a type of 'readonly' gives the client
		     its own personal 'db.have' database table.  A 'readonly'
		     client cannot 'edit' or 'submit' files, however for build
		     automation this is not usually a requirement and the
		     performance tradeoff is worth considering if your build
		     automation is causing issues with the 'db.have' table.
		     This option requires that an administrator has first
		     configured the 'client.readonly.dir' setting.  If it is
		     necessary to submit changes as part of your build,
		     you may specify a 'partitioned' client: like a 'readonly'
		     client, this type also has a separate 'db.have' table
		     under the 'client.readonly.dir' directory, but allows
		     journalled 'edit' and 'submit' of files.

	Note: changing the client root does not actually move the client
	files; you must relocate them manually.  Similarly, changing
	the 'LineEnd' option does not actually update the client files;
	you can refresh them with 'p4 sync -f'.

	The -d flag deletes the specified spec, as long as the client
	workspace has no opened files or pending changes.  (See 'p4 help
	opened'.) The -f flag forces the delete. The -Fs option used with -f
	forces client deletion even when the user has shelved changes. In
	this case, the shelved changes are left intact.

	The -o flag writes the named client spec to the standard output.
	The user's editor is not invoked.

	The -i flag reads a client spec from the standard input.  The
	user's editor is not invoked.

	The -t template flag, where 'template' is the name of another client
	spec, causes the View and Options fields to be replaced by those of
	the template.

	The -T type flag lets you set the type of client, this can only be
	set when first creating the client. For classic Perforce clients the
	default is 'writeable', set this value to 'readonly' for build
	automation or clients that do not need to update content.

	The -f flag can force the updating of locked clients; normally
	locked clients can only be modified by their owner.  -f also allows
	the last modified date to be set.  The -f flag requires 'admin'
	access granted by 'p4 protect'.

	The -s flag is used to switch an existing client spec's view without
	invoking the editor.  It can be used with -S to switch to a stream
	view, or with -t to switch to a view defined in another client spec.
	Switching views is not allowed in a client that has opened files.
	The -f flag can be used with -s to force switching with opened files.
	View switching has no effect on files in a client workspace until
	'p4 sync' is run.

	Without -s, the '-S stream' flag can be used to create a new client
	spec dedicated to a stream. If the client spec already exists, and
	-S is used without -s, an error occurs when the client is bound to
	a different, or no stream.  Using -S sets the new client's
	Stream field.  The special syntax '-S //a/stream@changelist' can
	be used to set both Stream and StreamAtChange at the same time.

	The '-S stream' flag can be used with '-o -c change' to inspect an
	old stream client view. It yields the client spec that would have
	been created for the stream at the moment the change was recorded.

	A server of type 'build-server' (see 'p4 help server') is a replica
	which supports build farm integration, and the 'p4 client' command
	may be used to create or edit client workspaces on a build-server.
	Such workspaces may issue the 'p4 sync' command, in addition to any
	read-only command supported by the replica. For more information, run
	'p4 help buildserver'.

	When creating or editing a client workspace for a build-server,
	the client specified by the optional 'name' argument, as well as the
	client specified by the $P4CLIENT environment variable or via the
	global '-c client' argument must not exist, or must be restricted
	to this server; this command may not be used to create or edit a
	workspace that is not restricted to this build-server.

	In order to delete a workspace which is bound to a different server,
	specify 'client -d -f --serverid=X', providing the serverid of the
	owning server. This variant of the client command must be issued
	directly to the Commit Server.

	See 'p4 help-graph client' for information on using this command with
	graph depots.
