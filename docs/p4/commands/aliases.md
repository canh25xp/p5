# p4 aliases


    aliases -- Display the list of aliases from the P4ALIASES file.

    p4 aliases

	The aliases file can be used to define command line aliases.

	When using the p4 command line tool, each command you enter is
	transformed by any alias definitions in your aliases file prior
	to being sent to the server for execution.

	Typical uses of aliases include: abbreviating complex commands;
	displaying command results in alternate output formats; automating
	simple multi-command sequences; and providing alternate syntax
	for commands you find hard to remember.

	The name of the aliases file is ~/.p4aliases on non-Windows platforms
	and %USERPROFILE%\p4aliases.txt on Windows platforms; this
	can be overridden using the P4ALIASES environment variable.

	Your aliases file can contain as many aliases as you want, in any
	order, specified one per line in the format:

	    alias = transformation

	Your aliases file can also contain comments: if the first non-blank
	character on a line is '#', the entire line is treated as a comment.

	When you issue a command which matches one of your aliases, the
	command is automatically modified by applying the transformation
	before issuing the command.

	Alias matching is case-sensitive even on otherwise case-insensitive
	platforms.

	The transformation can be a simple replacement:

	    commit = submit
	    halt = admin shutdown

	Certain characters in an alias definition have special meaning:
	    =      : separates the alias and its transformation
	    $(var) : denotes a named variable
	    &&     : separates two commands in a multi-command alias
	    >      : directs the output of a command to a variable
	    <      : directs the input of a command from a variable
	    \      : indicates the transformation continues on the next line

	The transformation can thus be more complex, and result in multiple
	commands issued to the server:

	    show-status = monitor show && lockstat && changes -m 1

	The transformation can include parameter substitutions:

	    myChanges = changes -u $(P4USER)

	The parameters can be arguments to the alias:

	    my-recent-changes $(max) = changes -u $(P4USER) -m $(max)

	The special built-in variables $(EQ), $(LT), and $(GT) expand
	to =, <, and >, respectively.

	Complex aliases are possible, and often involve using the output
	of one command as input to another:

	    show-last-change=-F "%change%" -ztag changes -m 1 \
	                         -s submitted > $(chg) &&
	                     describe -s $(chg)

	    reopenCL $(cl) = -F "%depotFile%" -ztag files @$(EQ)$(cl) \
	                         > $(files) && \
	                     -x - edit < $(files)
