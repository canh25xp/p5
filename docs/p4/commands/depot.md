# p4 depot


    depot -- Create or edit a depot specification

    p4 depot [-t type] name
    p4 depot -d [-f] name
    p4 depot -o name
    p4 depot -i

	Create a new depot specification or edit an existing depot
	specification. The specification form is put into a temporary file
	and the editor (configured by the environment variable $P4EDITOR)
	is invoked.

	The depot specification contains the following fields:

	Depot:       The name of the depot.  This name cannot be the same as
		     any branch, client, or label name.

	Owner:       The user who created this depot.

	Date:        The date that this specification was last modified.

	Description: A short description of the depot (optional).

	Type:        One of: 'local', 'stream', 'remote', 'spec', 'archive',
		     'unload', 'tangent', 'extension', 'graph', 'trait'.

		     A 'local' depot (the default) is managed directly by
		     the server and its files reside in the server's root
		     directory.

		     A 'stream' depot is a local depot dedicated to the
		     storage of files in a stream.

		     A 'remote' depot refers to files in another Perforce
		     server.

		     A 'spec' depot automatically archives all edited forms
		     (branch, change, client, depot, group, job, jobspec,
		     protect, triggers, typemap, and user) in special,
		     read-only files.  The files are named:
		     //depotname/formtype/name[suffix].  Updates to jobs made
		     by the 'p4 change', 'p4 fix', and 'p4 submit' commands
		     are also saved, but other automatic updates such as
		     as access times or opened files (for changes) are not.
		     A server can contain only one 'spec' depot.

		     An 'archive' depot defines a storage location to which
		     obsolete revisions may be relocated.

		     An 'unload' depot defines a storage location to which
		     database records may be unloaded and from which they
		     may be reloaded.

		     A 'tangent' depot defines a read-only location which
		     holds tangents created by the 'fetch -t' command. The
		     tangent depot named 'tangent' is automatically created
		     by 'fetch -t' if one does not already exist.

		     An 'extension' depot stores files related to Helix Core
		     Extensions.  See 'p4 help extension'.

		     A 'graph' depot defines a storage location to which
		     one or more git repositories are represented using
		     the git data model.

		     A 'trait' depot defines a storage location to which
		     huge traits may be stored. The storage location behavior
		     is determined by the 'p4 attribute' command or by the
		     setting of the 'trait.storedepot.min' configurable.
		     A server can contain only one 'trait' depot.
		     See 'p4 help attribute' and 'p4 help configurables'.
 
	Address:     For remote depots, the $P4PORT (connection address)
		     of the remote server.
		     For archive depots, this may be used to specify an S3
		     bucket as an alternative backing storage system.
		     See below.

	Suffix:      For spec depots, the optional suffix to be used
		     for generated paths. The default is '.p4s'.

	StreamDepth: For stream depots, the optional depth to be used
		     for stream paths in the depot, where depth equates
	             to the number of slashes following the depot name of a
	             stream.  This field is referenced when streams are
	             being created. The default is '1', matching the
	             traditional stream name.  This value may not be updated
	             once streams or archive data exist within the depot.

	Map:         Path translation information, in the form of a file
		     pattern with a single ... in it.  For local depots,
		     this path is relative to the server's root directory
		     or to server.depot.root if it has been configured
		     (Example: depot/...).  For remote depots, this path
		     refers to the remote server's namespace
		     (Example: //depot/...).

	SpecMap:     For spec depots, the optional description of which
	             specs should be saved, as one or more patterns.

	The -d flag deletes the specified depot.  If any files reside in the
	depot, they must be removed with 'p4 obliterate' before deleting the
	depot. If any archive files remain in the depot directory, they may
	be referenced by lazy copies in other depots; use 'p4 snap' to break
	those linkages. Snap lazy copies prior to obliterating the old depot
	files to allow the obliterate command to remove any unreferenced
	archives from the depot directory. If the depot directory is not
	empty, you must specify the -f flag to delete the depot.

	The -t flag is used to insert a value into the type field of a new
	depot spec.  This flag is ignored when used with existing depots.

	The -o flag writes the depot specification to standard output. The
	user's editor is not invoked.

	The -i flag reads a depot specification from standard input. The
	user's editor is not invoked.


	Storing the archives for a depot in S3 may be configured for archive
	depots by specifying the bucket configuration in the 'Address' field.
	The configuration must be provided in the form of ':' separated options
	and values on a single line, separated by commas (',') and preceded
	by 's3'. E.g:
	    Address: s3,bucket:mybucket,region:us-east-1

	The available options are:
	    - bucket    - The name of the bucket (required)
	    - url       - URL to the bucket
	                  (defaults to https://bucketname.s3.amazonaws.com)
	    - region    - The region the bucket resides in
	                  (optional depending on s3 implementation)
	    - accessKey - The S3 account's access key (required)
	    - secretKey - The S3 account's secret key (required)
	    - token     - The S3 account's session token
	                  (optional depending on bucket security)

	Note: Depending on several factors, including network latency and
	bandwidth, using S3 buckets for archive storage may not perform well
	enough to be practical.
