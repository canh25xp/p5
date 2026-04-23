# p4 where


    where -- Show how file names are mapped by the client view

    p4 where [file ...]

	Where shows how the specified files are mapped by the client view.
	For each argument, three names are produced: the name in the depot,
	the name on the client in Perforce syntax, and the name on the client
	in local syntax.

	If the file parameter is omitted, the mapping for all files in the
	current directory and below) is returned.

	Note that 'p4 where' does not determine where any real files reside.
	It only displays the locations that are mapped by the client view.
