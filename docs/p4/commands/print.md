# p4 print


    print -- Write a depot file to standard output

    p4 print [-a -A -K -o localFile -q -m max --offset offset --size size
	      -Q charset -B utf8bom -L line-ending] file[revRange] ...
    p4 print -U unloadfile ...

	Retrieve the contents of a depot file to the client's standard output.
	The file is not synced.  If file is specified using client syntax,
	Perforce uses the client view to determine the corresponding depot
	file.

	By default, the head revision is printed.  If the file argument
	includes a revision, the specified revision is printed.  If the
	file argument has a revision range,  then only files selected by
	that revision range are printed, and the highest revision in the
	range is printed. For details about revision specifiers, see 'p4
	help revisions'.

	The -a flag prints all revisions within the specified range, rather
	than just the highest revision in the range.

	The -A flag prints files in archive depots.

	The -K flag suppresses keyword expansion (this replaced the -k flag
	in 2022.1, which is now an alias for -K for backwards compatibility).

	The -L flag allows the line ending of textual files to be explicitly
	specified as either 'unix', 'win' or 'mac'.

	The -Q flag allows the charset for unicode type files to be explicitly
	specified, overriding the connection's P4CHARSET but not override the
	charset of unicode files with versioned charsets.

	The -B flag overrides the client-side 'filesys.utf8bom' setting,
	controlling the presence of the byte-order-mark in utf8 type files.

	The -o localFile flag redirects the output to the specified file on
	the client filesystem. Multiple files may be written by using wildcards
	in the localFile argument that match wildcards in the file argument.

	The -q flag suppresses the initial line that displays the file name
	and revision.

	The -m flag limits print to the first 'max' number of files.

	The --offset flag limits print by skipping 'offset' bytes from the
	begining of the file.

	The --size flag limits print to a maximum of 'size' bytes. Users can
	combine --offset and --size flags in one command.

	The -U option prints files in the unload depot (see 'p4 help unload'
	for more information about the unload depot).

	See 'p4 help-graph print' for information on using this command with
	graph depots.
