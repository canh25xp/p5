# p4 counter


     counter -- Display, set, or delete a counter

     p4 counter name
     p4 counter [-f -v] name value
     p4 counter [-f] -d name
     p4 counter [-f -v] -i name
     p4 counter [-f] -m [ pair list ]
     p4 counter --from=oldValue --to=newValue name

	The first form displays the value of the specified counter.

	The second form sets the counter to the specified value.

	The third form deletes the counter.  This option usually has the
	same effect as setting the counter to 0.

	The -f flag sets or deletes counters used by Perforce,  which are
	listed by 'p4 help counters'. Important: Never set the 'change'
	counter to a value that is lower than its current value.

	The -i flag increments a counter by 1 and returns the new value.
	This option is used instead of a value argument and can only be
	used with numeric counters.

	The -v flag displays the previous value of the specified counter
	after the counter has been set or incremented.

	The -m flag in the fifth form allows multiple operations in one
	command. With this form, the list is pairs of arguments.  Each pair
	is either counter value or '-' counter.  To set a counter use a name
	and value. To delete a counter use a '-' followed by the name.

	The sixth form sets a counter to the newValue only if the current
	value of the counter is oldValue. Note that a counter that has never
	been set (or has been deleted) cannot be set using this form.

	Counters can be assigned textual values as well as numeric ones,
	despite the name 'counter'.

	'p4 counter' requires 'review' access granted by 'p4 protect'.
	The -f flag requires that the user be an operator or have 'super'
	access.
