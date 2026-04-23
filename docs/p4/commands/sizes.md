# p4 sizes


    sizes -- Display information about the size of the files in the depot

    p4 sizes [-a -S] [-s | -z] [-b size] [-h|-H] [-m max] file[revRange] ...
    p4 sizes -C [-K] [-Q charset] [-B utf8bom] [-L line-ending] [-a -S]
                [-h|-H] [-m max] file[revRange] ...
    p4 sizes -A [-a] [-s] [-b size] [-h|-H] [-m max] archivefile...
    p4 sizes -U unloadfile ...

	For specified file specification, displays depot file name, revision,
	file count and file size.  If you use client syntax for the file
	specification,the view mapping is used to list the corresponding
	depot files.

	The -a flag lists all revisions within the specific range, rather
	than just the highest revision in the range.

	The -b option specifies the blocksize in bytes.  When this option
	is specified, each accumulated filesize is rounded up to the nearest
	blocksize.

	The -h or -H flag prints the sizes in human-readable form, using a
	scaling factor of 1024 for -h or 1000 for -H.

	The -m flag limits sizes to the first 'max' number of files.

	The -s option calculates the sum of the file sizes for the specified
	files.

	The -S option displays size information for shelved files only. With
	this option, revision specifications are not permitted.

	The -z option omits lazy copies when calculating the file sizes for
	the specified files.

	If there is no file size information for a file revision in the depot
	metadata, the sizes command returns a size of "<n/a>".

	'p4 verify -u' updates the metadata for all revisions that require it.

	The -C flag reports the file size as expected in the workspace,
	defaulting to the client's environment, accounting for line-endings,
	charset conversion of unicode/utf8/utf16 filetypes (including the BOM
	for utf8 type files) and keyword expansion for +k type files.
	These variables can be explicitly overridden with the following flags:
	   -B utf8bom     - Override 'filesys.utf8bom' for utf8 type files
	   -K             - Collapse keywords for +k type files
	   -L line-ending - Override the client's line ending setting
	   -Q charset     - Override P4CHARSET for unicode type files

	Where client file size information would not match the server's, the
	size is calculated server-side on request and cached for future use.

	The -A flag displays files in archive depots (see 'p4 help archive').

	The -U option displays size information for unload files in the
	unload depot (see 'p4 help unload').
