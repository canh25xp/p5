# p4 annotate


    annotate -- Print file lines and their revisions

    p4 annotate [-aciIqtTu -d<flags>] file[revRange] ...

	Prints all lines of the specified files, indicating the revision that
	introduced each line into the file.

	If the file argument includes a revision, then only revisions up to
	the specified revision are displayed.  If the file argument has a
	revision range, only revisions within that range are displayed. For
	details about specifying revisions, see 'p4 help revisions'.

	The -a flag includes both deleted files and lines no longer present
	at the head revision. In the latter case, both the starting and ending
	revision for each line is displayed.

	The -c flag directs the annotate command to output changelist numbers
	rather than revision numbers for each line.

	The -d<flags> change the way whitespace and/or line endings are
	treated: -db (ignore whitespace changes), -dw (ignore whitespace),
	-dl (ignore line endings).

	The -i flag follows branches.  If a file was created by branching,
	'p4 annotate' includes the revisions of the source file up to the
	branch point, just as 'p4 filelog -i' does.  If a file has history
	prior to being created by branching (such as a file that was branched
	on top of a deleted file), -i ignores those prior revisions and
	follows the source.  -i implies -c.

	The -I flag follows all integrations into the file.  If a line was
	introduced into the file by a merge, the source of the merge is
	displayed as the changelist that introduced the line. If the source
	itself was the result of an integration, that source is used instead,
	and so on.  -I implies -c and may not be combined with -i.

	The -q flag suppresses the one-line header that is displayed by
	default for each file.

	The -t flag forces 'p4 annotate' to display binary files.

	The -T flag aligns output to a tab stop of 8.  A different tab stop
	may be specified via '--tab=N'.

	The -u flag directs the annotate command to output the user who
	modified the line in the file and the date it was modified.
