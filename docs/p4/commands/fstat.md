# p4 fstat


    fstat -- Dump file info

    p4 fstat [-F filter -L -T fields -m max -r] [-c | -e changelist#]
	[-Ox -Rx -Sx] [-A pattern] [-U] file[rev] ...
    p4 fstat [-F filter -T fields -m max -r] [-c | -e changelist#]
	[-Ox -Rx -Sx] [-A pattern] [ --streamviews ] file[rev] ...

	fstat lists information about files, one line per field.  Fstat is
	intended for use in Perforce API applications, where the output can
	be accessed as variables, but its output is also suitable for parsing
	from the client command output in scripts.

	The fields that fstat displays are:

		attr-<name>          -- attribute value for <name>
		attrProp-<name>      -- set if attribute <name> is propagating
		clientFile           -- local path (host or Perforce syntax)
		depotFile            -- name in depot
		movedFile            -- name in depot of moved to/from file
		path                 -- local path (host syntax)
		isMapped             -- set if file is mapped in the client
		isMissing            -- set if the rev is known to not be in
		                        the depot
		shelved              -- set if file is shelved
		headAction           -- action at head rev, if in depot
		headChange           -- head rev changelist#, if in depot
		headRev              -- head rev #, if in depot
		headType             -- head rev type, if in depot
		headCharset          -- head charset, for unicode type
		headTime             -- head rev changelist time, if in depot
		headModTime	     -- head rev mod time, if in depot
		movedRev             -- head rev # of moved file
		haveRev              -- rev had on client, if on client
		desc                 -- change description (if -e specified)
		digest               -- MD5 digest (fingerprint)
		fileSize             -- file size
		otherOpen            -- set if someone else has it open
		otherOpen#           -- list of user@client with file opened
		otherLock            -- set if someone else has it locked
		otherLockGlobal      -- set if lock is global
		otherLockOnCommit    -- set if locked on commit server
		otherLock#           -- user@client with file locked
		otherLockGlobal#     -- user@client with file globally locked
		otherLockOnCommit#   -- user@client with lock on commit server
		otherAction#         -- open action, if opened by someone else
		otherChange#         -- changelist, if opened by someone else
		lockGlobal           -- set if globally locked
		lockOnCommit         -- set if locked on commit server
		openattr-<name>      -- attribute value for <name>
		openattrProp-<name>  -- set if attribute <name> is propagating
		totalFileCount       -- total no. of files, if sorted

	If the file is opened in the current workspace, additional fields
	that fstat displays are:

		action               -- open action, if opened
		actionOwner          -- user who opened file, if opened
		change               -- open changelist#, if opened
		charset              -- open charset, for unicode type
		ourLock              -- set if this user/client has it locked
		revtime              -- file modification time, if shelved
		type                 -- open type, if opened
		workRev              -- open revision, if opened

	If the opened file involves pending integrations, additional fields
	that fstat displays are:

		resolved             -- resolved integration records
		unresolved           -- unresolved integration records
		reresolvable         -- reresolvable integration records
		resolveAction#       -- pending integration record action
		resolveType#         -- pending integration type
		resolveBaseFile#     -- pending integration base file
		resolveBaseRev#      -- pending integration base rev
		resolveFromFile#     -- pending integration from file
		resolveStartFromRev# -- pending integration from start rev
		resolveEndFromRev#   -- pending integration from end rev

	The -A <pattern> flag restricts displayed attributes to those that
	match 'pattern'.

	The -F flag lists only files satisfying the filter expression. This
	filter syntax is similar to the one used for 'jobs -e jobview' and is
	used to evaluate the contents of the fields in the preceding list.
	Filtering is case-sensitive.

	        Example: -Ol -F "fileSize > 1000000 & headType=text"

	Note: filtering is not optimized with indexes for performance.

	The -L flag can be used with multiple file arguments that are in
	full depot syntax and include a valid revision number. When this
	flag is used the arguments are processed together by building an
	internal table similar to a label. This file list processing is
	significantly faster than having to call the internal query engine
	for each individual file argument. However, the file argument syntax
	is strict and the command will not run if an error is encountered.

	The -T fields flag returns only the specified fields. The field names
	can be specified using a comma- or space-delimited list.

	        Example: -Ol -T "depotFile, fileSize"

	The -m max flag limits output to the specified number of files.

	The -r flag sorts the output in reverse order.

	The -c changelist# flag displays files modified by the specified
	changelist or after that changelist was submitted.  This operation is
	much faster than using a revision range on the affected files.

	The -e changelist# flag lists files modified by the specified
	changelist. When used with the -Ro flag, only pending changes are
	considered, to ensure that files opened for add are included. This
	option also displays the change description.

	The -O options modify the output as follows:

	        -Oa     output attributes set by 'p4 attribute'

		-Ob	output the path, revision, type, full and relative
			local paths of the server archive file. Requires
			'admin' privilege.

	        -Od     output the digest of the attribute

	        -On     output attribute value storage locations rather than
	                the attribute values

	        -Oe     output attribute values encoded as hex

	        -Of     output all revisions for the given files (this
	                option suppresses other* and resolve* fields)

	        -Ol     output a fileSize and digest field for each revision
	                (this may be expensive to compute)

	        -Op     output the local file path in both Perforce syntax
	                (//client/) as 'clientFile' and host form as 'path'

	        -Or     output pending integration record information for
	                files opened on the current client, or if used with
	                '-e <change> -Rs', on the shelved change

	        -Os     exclude client-related data from output

	        -OL     when run against an edge server, output global lock
	                information from the commit server

	The -R option limits output to specific files:

	        -Rc     files mapped in the client view
	        -Rh     files synced to the client workspace
	        -Rn     files opened not at the head revision
	        -Ro     files opened
	        -Rr     files opened that have been resolved
	        -Rs     files shelved (requires -e)
	        -Ru     files opened that need resolving

	The -S option changes the order of output:

	        -Sd     sort by date
	        -Sh     sort by have revision
	        -Sr     sort by head revision
	        -Ss     sort by filesize
	        -St     sort by filetype

	Note that sorting is applied to all potential results matching
	the file argument(s), and the totalFileCount field indicates
	the number of results before the -m option is applied.

	The -U flag displays information about unload files in the unload
	depot (see 'p4 help unload').

	For compatibility, the following flags are also supported:
	-C (-Rc) -H (-Rh) -W (-Ro) -P (-Op) -l (-Ol) -s (-Os).

	The --streamviews option reports results for streams in the context
	of the stream.

	See 'p4 help-graph fstat' for information on using this command with
	graph depots.
