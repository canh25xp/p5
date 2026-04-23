# p4 reshelve


    reshelve -- Copy shelved files to a new or existing shelf.

    p4 reshelve [-p] -s changelist# [file ...]
    p4 reshelve [-f] [-p] -s changelist# -c changelist# [file ...]

	'p4 reshelve' copies shelved files from an existing shelf into
	either a new shelf or one that has already been created. This
	command leaves the source shelf intact.

	If a file pattern is specified, 'p4 reshelve' shelves the files that
	match the pattern.

	The -s flag specifies the shelved changelist that contains the
	shelved files to be copied.

	The -c flag specifies the pending changelist that will be the target
	for the shelved files rather than creating a new one. To update
	a target shelf you must be the owner of the changelist.

	The -f (force) flag must be used when the same file already exists in
	the target changelist in order to overwrite it.

	The -p flag promotes the new or target changelist where it can be
	accessed by other edge servers participating in the distributed
	configuration.  Once a shelved change has been promoted, all
	subsequent local modifications to the shelf are also pushed to
	the commit server and remain until the shelf is deleted.
