# p4 protect


    protect -- Modify protections in the server namespace

    p4 protect [path]
    p4 protect -o [path]
    p4 protect -i [path]

	'p4 protect' edits the protections table in a text form.

	Each line in the table contains a protection mode, a group/user
	indicator, the group/user name, client host ID and a depot file
	path pattern. Users receive the highest privilege that is granted
	on any line.

	Note: remote depots are accessed using the pseudo-user 'remote'.
	To control access from other servers that define your server as
	a remote server, grant appropriate permissions to the 'remote' user.

	    Mode:    The permission level or right being granted or denied.
		     Each permission level includes all the permissions above
		     it, except for 'review' and 'owner'. Each permission only
		     includes the specific right and no lesser rights.  This
		     approach enables you to deny individual rights without
		     having to re-grant lesser rights. Modes prefixed by '='
		     are rights. All other modes are permission levels.

		     'readstreamspec', 'openstreamspec' and 'writestreamspec'
		     determine access to stream spec definitions.
		     If no streamspec permission exists for any user, then
		     'list', 'open' and 'write' permissions control
		     stream spec access for 'edit', 'resolve', 'revert',
		     'shelve', 'submit', 'streams', and 'stream' commands.
		     'list' grants 'p4 streams' access for stream spec paths.
		     Any 'open' or higher permssion for a user anywhere grants
		     stream spec 'edit' and 'write' permissions to that user
		     for all stream specs.

		     If any streamspec permissions exist for any user, then
		     'list' will continue to provide 'p4 streams' access.
		     However, read/open/write access to stream specs will be
		     controlled by 'readstreamspec', 'openstreamspec', and
		     'writestreamspec'. Thus, users without explicit streamspec
		     permissions have no stream spec read/open/write access.

		     Note: streamspec permissions are disabled by default.
		     Enable by setting the server configurable
                     dm.protects.streamspec to 1.

		     Valid modes are:

		     list    - users can see names but not contents of files;
			       users can see all non-file related metadata
			       (clients, users, changelists, jobs, etc.)
			       also provides 'p4 streams' access

		     read    - users can sync, diff, and print files

		     open    - users can open files (add, edit, delete,
			       integrate)

		     write   - users can submit open files

		     admin   - permits those administrative commands and
			       command options that don't affect the server's
			       security.

		     owner   - allows access to the 'p4 protect' command, for
			       the specified path.

		     super   - access to all commands and command options.

		     review  - permits access to the 'p4 review' command;
			       implies read access

		     =read   - if this right is denied, users can't sync,
			       diff, or print files

		     =branch - if this right is denied, users are not
			       permitted to use files as a source
			       for 'p4 integrate'

		     =open   - if this right is denied, users cannot open
			       files (add, edit, delete, integrate)

		     =write  - if this right is denied, users cannot submit
			       open files

		     readstreamspec   - users can display a stream spec with
		                        with 'p4 stream -o'.

		     openstreamspec   - users can revert, resolve, shelve, or
		                        open for edit a stream spec

		     writestreamspec  - users can submit or modify a stream
		                        spec

		     =readstreamspec  - if this right is denied, users cannot
		                        execute 'p4 stream -o'

		     =openstreamspec  - if this right is denied, users cannot
		                        revert, resolve, shelve, or open for
		                        edit a stream spec

		     =writestreamspec - if this right is denied, users cannot
		                        submit or modify a stream spec

	    Group/User indicator: specifies the grantee is a group or user.

	    Name:    A Perforce group or user name; can include wildcards.

	    Host:    The IP address of a client host. IPv6 and IPv4 addresses
	             are supported and the * wildcard can be used to refer to
	             all IP addresses. When using the * wildcard with an IPv6
	             address, enclose the entire address in square brackets.
	             Best practice is to use CIDR notation, surround IPv6
	             addresses with brackets, and to avoid the * wildcard.

	             The server can distinguish connections coming from a
	             proxy, broker, or replica. The server prepends the string
	             'proxy-' to the IP address of the true client of such
	             a connection when the server enforces the protections.

	             Specify the 'proxy-' prefix for the IP address in the
	             Host: field in the protections table to indicate the
	             protections that should thus apply.

	             For example, 'proxy-*' applies to all connections from
	             all proxies, brokers, and replicas, while
	             'proxy-10.0.0.5' identifies a client machine with an IP
	             address of 10.0.0.5 which is connecting to p4d through
	             a proxy, broker, or replica.

	             If you wish to write a single set of protections entries
	             which apply both to directly-connected clients as well
	             as to those which connect via a proxy, broker, or
	             replica, you can omit the 'proxy-' prefix and also set
	             dm.proxy.protects=0. In this case, the 'proxy-' prefix
	             is not prepended to the IP address of connections which
	             are made via a proxy, replica or broker.  Note that in
	             this scenario, all intermediate proxies, brokers, and
	             replicas should be at release 2012.1 or higher.

	    Path:    The part of the depot to which access is being granted
	             or denied.  To deny access to a depot path, preface the
	             path with a "-" character. These exclusionary mappings
	             apply to all access levels, even if only one access
	             level is specified in the first field.


	There are two additional fields in the protect specification that are
	not part of the protections table:

	    SubPath: The root path of the sub-protections table (only used when
	             editing a sub-protections table).

	    Update:  The date this specification was last modified (read-only).
	             If this date is modified, the protections table will not
	             be saved; however, ommitting this field will avoid the
	             modification check.


	Sub-protections tables:
	    It is possible to delegate management of parts of the protections
	    table to non-super users or groups by creating an entry with the
	    mode 'owner'.  These entries must have a unique path, without
	    wildcards except for a trailing '...'.  Users with 'super' or that
	    have been granted 'owner' for a path may run the 'p4 protect'
	    command specifying the granted path as an argument to access the
	    sub-protections table for that path.  Any entries in this table
	    will be appended to the effective protections table directly below
	    the 'owner' entry; if an 'owner' entry is removed, so are any
	    entries in the sub-protections table for that path.  The 'owner'
	    permission only grants access to the sub-protections table at the
	    specified path: it does not grant the specified user or group any
	    permissions on that path.  The owner may grant themselves 'admin'
	    permissions on that path.  Neither 'owner' nor 'super' entries may
	    be added to a sub-protections table, and any other entries' paths
	    must be within the scope of the sub-protections table's path.


	If a path argument is specified, and an 'owner' entry with the same
	path exists, then the sub-protections table for that path will be
	accessed instead of the main protections table.

	The -o flag writes the protection table to the standard output.
	The user's editor is not invoked.

	The -i flag reads the protection table from the standard input.
	The user's editor is not invoked.

	After protections are defined, 'p4 protect' requires 'super' or 'owner'
	access.
