# p4 attribute


    attribute -- Set per-revision attributes on revisions

    p4 attribute [-e -f -p] -n name [-v value [-T0|-T1]] files...
    p4 attribute [-e -f -p [-T0|-T1]] -i -n name file

	Sets a user-managed attribute 'name' to 'value' on opened files.
	If -v is omitted, the attribute is cleared (if set).

	To display attributes use 'p4 fstat -Oa'.

	The -e flag indicates that the 'value' is specified in hex.

	The -f flag sets the attribute on submitted files.  If a propagating
	trait is set on a submitted file, a revision specification may not
	be used, and the file must not be currently open on any client.
	The -f flag requires 'admin' access granted by 'p4 protect'.

	The -p flag creates an attribute with a value that is propagated
	when the file is opened with 'p4 copy', 'p4 delete', 'p4 edit',
	'p4 integrate', 'p4 reconcile', 'p4 resolve', 'p4 submit', 'p4 shelve',
	or 'p4 unshelve'.

	The -i flag causes the attribute value to be read from the standard
	input. Only one file argument is allowed when using this option.

	Multiple attributes can be set or cleared by specifying multiple
	'-n name' flags and an equal number of corresponding '-v value'
	flags (to set) or no '-v' flags (to clear).

	When an attribute is created, the configurable 'trait.storagedepot.min'
	determines how the attribute is stored. By default, attribute values
	are stored in the db.traits table. The 'trait.storagedepot.min'
	value is used to enable depot storage for new attribute values, and
	also to specify the minimum size in bytes for storing a value in the
	trait depot. When 'trait.storagedepot.min' is set to a value greater
	than 0, the value of a newly created attribute will be stored in the
	trait depot if its size matches or exceeds the trait.storagedepot.min
	value. When 'trait.storagedepot.min' is unset or set to 0, trait values
	are stored in the 'db.traits' table instead.

	The -T option may be used when creating a new attribute to override
	the configurable setting only when the 'trait.storagedepot.min'
	configurable is set to a value greater than 0. The -T0 setting causes
	the value to be stored in the db.traits table. When trait depot
	storage is allowed, the -T1 option specifies that the value will be
	stored in the trait depot even if it does not meet the
	trait.storagedepot.min size.
