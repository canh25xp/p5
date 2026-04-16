# p4 key


     key -- Display, set, or delete a key/value pair

     p4 key name
     p4 key [-v] name value
     p4 key [-d] name
     p4 key [-i -v] name
     p4 key -m [ pair list ]
     p4 key --from=oldValue --to=newValue name

	The first form displays the value of the specified key.

	The second form sets the key to the specified value.

	The third form deletes the key.

	The -i flag increments a keys value by 1 and returns the new value.
	This option is used instead of a value argument and can only be
	used with numeric key values.

	The -v flag displays the previous value of the specified key
	after the key has been set or incremented.

	The -m flag in the fifth form allows multiple operations in one
	command. With this form, the list is pairs of arguments.  Each pair
	is either key value or '-' key.  To set a key use a key
	and value. To delete a key use a '-' followed by the key.

	The sixth form sets a key to the newValue only if the current
	value of the key is oldValue. Note that a key that has never
	been set (or has been deleted) cannot be set using this form.

	'p4 key' requires 'review' access granted by 'p4 protect'.
