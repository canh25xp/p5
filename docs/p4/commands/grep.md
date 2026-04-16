# p4 grep


    grep -- Print lines matching a pattern

    p4 grep [options] -e pattern file[revRange]...

	options: -a -i -n -A <num> -B <num> -C <num> -t -s (-v|-l|-L) (-F|-G)

	Searches files for lines that match the specified regular expression,
	which can contain wildcards.  The parser used by the Perforce server
	is based on V8 regexp and might not be compatible with later parsers,
	but the majority of functionality is available.

	By default the head revision is searched.  If the file argument
	includes a revision specification, all corresponding revisions are
	searched.  If the file argument includes a revision range, only files
	in that range are listed, and the highest revision in the range is
	searched.  For details about revision specifiers, see
	'p4 help revisions'.

	The -a flag searches all revisions within the specified range. By
	default only the highest revision in the range is searched.

	The -i flag causes the pattern matching to be case-insensitive. By
	default, matching is case-sensitive.

	The -n flag displays the matching line number after the file revision
	number. By default, matches are displayed as revision#: <text>.

	The -v flag displays files with non-matching lines.

	The -F flag is used to interpret the pattern as a fixed string.

	The -G flag is used to interpret the pattern as a regular expression,
	which is the default behavior.

	The -L flag displays the name of each selected file from which no
	output would normally have been displayed. Scanning stops on the
	first match.

	The -l flag displays the name of each selected file containing
	matching text. Scanning stops on the first match.

	The -s flag suppresses error messages that result from abandoning
	files that have a maximum number of characters in a single line that
	are greater than 4096.  By default, an error is reported when grep
	abandons such files.

	The -t flag searches binary files.  By default, only text files are
	searched.

	The -A <num> flag displays the specified number of lines of trailing
	context after matching lines.

	The -B <num> flag displays the specified number of lines of leading
	context before matching lines.

	The -C <num> flag displays the specified number of lines of output
	context.

	Regular expressions:

	A regular expression is zero or more branches, separated by `|'. It
	matches anything that matches one of the branches.

	A branch is zero or more pieces, concatenated.  It matches a match
	for the first, followed by a match for the second, etc.

	A piece is an atom possibly followed by `*', `+', or `?'.  An atom
	followed by `*' matches a sequence of 0 or more matches of the atom.
	An atom followed by `+' matches a sequence of 1 or more matches of
	the atom.  An atom followed by `?' matches a match of the atom, or
	the null string.

	An atom is a regular expression in parentheses (matching a match for
	the regular expression),  a range (see below),  `.'  (matching any
	single character),  `^' (matching the null string at the beginning
	of the input string),  `$' (matching the null string at the end of
	the input string),  a `\' followed by a single character (matching
	that character),  or a single character with no other significance
	(matching that character).

	A range is a sequence of characters enclosed in `[]'.  It normally
	matches any single character from the sequence.  If the sequence
	begins with `^',  it matches any single character not from the rest
	of the sequence.  If two characters in the sequence are separated by
	`-', this is shorthand for the full list of ASCII characters between
	them (e.g. `[0-9]' matches any decimal digit).  To include a literal
	`]' in the sequence, make it the first character (following a possible
	`^').  To include a literal `-', make it the first or last character.

	The symbols '\<' and '\>' respectively match the empty string at
	the beginning and end of a word.
