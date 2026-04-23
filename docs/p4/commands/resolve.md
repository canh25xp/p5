# p4 resolve


    resolve -- Resolve integrations and updates to workspace files

    p4 resolve [options] [file ...]
    p4 resolve -So [-a<flag>] [-n] [-o]

	options: -A<flags> -a<flags> -d<flags> -f -K -n -N -o -t -v
		 -c changelist#

	'p4 resolve' resolves changes to files in the client workspace.

	'p4 resolve' works only on files that have been scheduled to be
	resolved.  The commands that can schedule resolves are: 'p4 sync',
	'p4 update', 'p4 submit', 'p4 merge', and 'p4 integrate'.  Files must
	be resolved before they can be submitted.

	'p4 resolve -So' is an alias for 'p4 stream resolve'.
	See 'p4 help streamcmds'.

	Resolving involves two sets of files, a source and a target.  The
	target is a set of depot files that maps to opened files in the
	client workspace.  When resolving an integration, the source is a
	different set of depot files than the target.  When resolving an
	update, the source is the same set of depot files as the target,
	at a different revision.

	The 'p4 resolve' file argument specifies the target.  If the file
	argument is omitted, all unresolved files are resolved.

	Resolving can modify workspace files. To back up files, use 'p4
	shelve' before using 'p4 resolve'.

	The resolve process is a classic three-way merge. The participating
	files are referred to as follows:

	  'yours'       The target file open in the client workspace
	  'theirs'      The source file in the depot
	  'base'        The common ancestor; the highest revision of the
	                source file already accounted for in the target.
	  'merged'      The merged result.

	Filenames, filetypes, and text file content can be resolved by
	accepting 'yours', 'theirs', or 'merged'.  Branching, deletion, and
	binary file content can be resolved by accepting either 'yours' or
	'theirs'.

	When resolving integrated changes, 'p4 resolve' distinguishes among
	four results: entirely yours, entirely theirs, a pure merge, or an
	edited merge.  The distinction is recorded when resolved files are
	submitted, and will be used by future commands to determine whether
	integration is needed.

	In all cases, accepting 'yours' leaves the target file in its current
	state.  The result of accepting 'theirs' is as follows:

	   Content:     The target file content is overwritten.
	   Attribute:   The target's attributes are replaced.
 	   Branching:	A new target is branched.
 	   Deletion:    The target file is deleted.
 	   Filename:	The target file is moved or renamed.
 	   Filetype:    The target file's type is changed.

	For each unresolved change, the user is prompted to accept a result.
	Content and non-content changes are resolved separately.  For content,
	'p4 resolve' places the merged result into a temporary file in the
	client workspace.  If there are any conflicts, the merged file contains
	conflict markers that must be removed by the user.

	'p4 resolve' displays a count of text diffs and conflicts, and offers
	the following prompts:

	  Accept:
	     at              Keep only changes to their file.
	     ay              Keep only changes to your file.
	   * am              Keep merged file.
	   * ae              Keep merged and edited file.
	   * a               Keep autoselected file.

	  Diff:
	   * dt              See their changes alone.
	   * dy              See your changes alone.
	   * dm              See merged changes.
	     d               Diff your file against merged file.

	  Edit:
	     et              Edit their file (read only).
	     ey              Edit your file (read/write).
	   * e               Edit merged file (read/write).

	  Misc:
	   * m               Run '$P4MERGE base theirs yours merged'.
			     (Runs '$P4MERGEUNICODE charset base theirs
			      yours merged' if set and the file is a
			      unicode file.)
	     s               Skip this file.
	     h               Print this help message.
	     ^C              Quit the resolve operation.

	Options marked (*) appear only for text files. The suggested action
	will be displayed in brackets.

	The 'merge' (m) option enables you to invoke your own merge program, if
	one is configured using the $P4MERGE environment variable.  Four files
	are passed to the program: the base, yours, theirs, and the temporary
	file. The program is expected to write merge results to the temporary
	file.

	The -A flag can be used to limit the kind of resolving that will be
	attempted; without it, everything is attempted:

	    -Aa		Resolve attributes.
	    -Ab		Resolve file branching.
	    -Ac		Resolve file content changes.
	    -Ad		Resolve file deletions.
	    -Am		Resolve moved and renamed files.
	    -At		Resolve filetype changes.
	    -AQ		Resolve charset changes.

	The -a flag puts 'p4 resolve' into automatic mode. The user is not
	prompted, and files that can't be resolved automatically are skipped:

	    -as		'Safe' resolve; skip files that need merging.
	    -am 	Resolve by merging; skip files with conflicts.
	    -af		Force acceptance of merged files with conflicts.
	    -at		Force acceptance of theirs; overwrites yours.
	    -ay		Force acceptance of yours; ignores theirs.

	The -as flag causes the workspace file to be replaced with their file
	only if theirs has changed and yours has not.

	The -am flag causes the workspace file to be replaced with the result
	of merging theirs with yours. If the merge detected conflicts, the
	file is left untouched and unresolved.

	The -af flag causes the workspace file to be replaced with the result
	of merging theirs with yours, even if there were conflicts.  This can
	leave conflict markers in workspace files.

	The -at flag resolves all files by copying theirs into yours. It
	should be used with care, as it overwrites any changes made to the
	file in the client workspace.

	The -ay flag resolves all files by accepting yours and ignoring
	theirs. It preserves the content of workspace files.

	Note that there is a subtle distinction between manually accepting a
	merge resolve ('p4 resolve' and selecting 'am') and automatic merge
	resolve ('p4 resolve -am'). When accepting a merge resolve, the user
	makes a conscious choice.  With automatic merge resolves, the choice is
	automated and the result will depend on the three way merge differences
	between the target and source files.

	With an automatic merge resolve, if there are only additions or
	deletions in the target file, the target differences will be kept (the
	same as a manual accept yours).  If there are only additions or
	deletions in the source file, the source differences will be kept (the
	same as a manual accept theirs).  If there are additions or deletions
	in both the source and target files and there are no conflicts, the
	merged result is kept.

	In other words, 'p4 resolve -am' does not guarantee that all source
	and target files will be merged, only that the choice of whether to
	accept yours, accept theirs, or accept merge will be automated based
	upon the three way merge differences described above.

	The -d flags can be used to control handling of whitespace and line
	endings when merging files:

	    -db		Ignore whitespace changes.
	    -dw		Ignore whitespace altogether.
	    -dl 	Ignores line endings.

	The -d flags are also passed to the diff options in the 'p4 resolve'
	dialog. Additional -d flags that modify the diff output but do not
	modify merge behavior include -dn (RCS), -dc (context), -ds (summary),
	and -du (unified). Note that 'p4 resolve' uses content from the depot
	file if the files differ only in whitespace.

	The -f flag enables previously resolved content to be resolved again.
	By default, after files have been resolved, 'p4 resolve' does not
	process them again. The content of the target (yours) file being
	re-resolved is the result of the previous resolve, not the content of
	the original file. To preserve the option of using the original file,
	revert the resolved file.

	The -K flag suppresses keyword expansion when updating ktext files on
	the client.

	The -n flag previews the operation without altering files.

	The -N flag previews the operation with additional information about
	any non-content resolve actions that are scheduled.

	The -o flag displays the base file name and revision to be used
	during the merge.

	The -t flag forces 'p4 resolve' to attempt a textual merge, even for
	files with non-text (binary) types.

	The -v flag causes 'p4 resolve' to insert markers for all changes,
	not just conflicts.

	The -c flag limits 'p4 resolve' to the files in changelist#.

	See 'p4 help-graph resolve' for information on using this command with
	graph depots.
