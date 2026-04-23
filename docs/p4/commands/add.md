# p4 add


    add -- Open a new file to add it to the depot

    p4 add [-c changelist#] [-d -f -I -n] [-t filetype] file ...

	Open a file for adding to the depot.  If the file exists on the
	client, it is read to determine if it is text or binary. If it does
	not exist, it is assumed to be text.  To be added, the file must not
	already reside in the depot, or it must be deleted at the current
	head revision.  Files can be deleted and re-added.

	A 2012.1 client will ignore files that were to be added, if they
	match an exclusion line specified in a P4IGNORE file.

	To associate the open files with a specific pending changelist, use
	the -c flag; if you omit the -c flag, the open files are associated
	with the default changelist.

	As a shortcut to reverting and re-adding, you can use the -d
	flag to reopen currently-open files for add (downgrade) under
	the following circumstances:

	    A file that is 'opened for edit' and is synced to the head
	    revision, and the head revision has been deleted (or moved).

	    A file that is 'opened for move/add' can be downgraded to add,
	    which is useful when the source of the move has been deleted
	    or moved.  Typically, under these circumstances, your only
	    alternative is to revert.  In this case, breaking the move
	    connection enables you to preserve any content changes in the
	    new file and safely revert the source file (of the move).

	To specify file type, use the -t flag.  By default, 'p4 add'
	determines file type using the name-to-type mapping table managed
	by 'p4 typemap' and by examining the file's contents and execute
	permission bit. If the file type specified by -t or configured in
	the typemap table is a partial filetype, the resulting modifier is
	applied to the file type that is determined by 'p4 add'. For more
	details, see 'p4 help filetypes'.

	To add files with filenames that contain wildcard characters, specify
	the -f flag. Filenames that contain the special characters '@', '#',
	'%' or '*' are reformatted to encode the characters using ASCII
	hexadecimal representation.  After the files are added, you must
	refer to them using the reformatted file name, because Perforce
	does not recognize the local filesystem name.

	The -I flag informs the client that it should not perform any ignore
	checking configured by P4IGNORE.

	The -n flag displays a preview of the specified add operation without
	changing any files or metadata.

	See 'p4 help-graph add' for information on using this command with
	graph depots.
