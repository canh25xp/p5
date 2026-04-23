# p4 streamspec


    streamspec -- Edit the stream template

    p4 streamspec
    p4 streamspec -o
    p4 streamspec -i

	streamspec edits the template that specifies the format of streams.
	This format is used by 'p4 stream' when streams are entered or updated,
	and by 'p4 streams'.

	streamspec brings up a form with the following fields:

	   Fields:  A list of the fields maintained for each stream, one
		    line per field.  Each line has five words: code, name,
		    data-type, len, and field-type.

		    'code' is a unique integer identifier for storing
			the data of the field. When adding a new field,
			one may either enter a numeric field code value
			in the 701-749 range, or as 'NNN'. In the later
			case, the next available code value will be
			automatically generated. If the number of fields
			exhausts the range, new codes are assigned unique
			values greater than or equal to 10000.

		    'name' is the name of the field for the stream.

		    'data-type' indicates the format of the field:

			word: a single word (any value)
			date: a date/time field
			select: one of a set of words
			line: a one-liner
			text: a block of text

		    'len' is the recommended character length of a
			display box for the field.  If 0, a text box is
			assumed.

		    'field-type' indicates how to handle the setting of
			the field:

			optional: no default, and not required to be present
			default: default provided, still not required
			required: default provided, value must be present
			once: set once to the default and never changed
			always: always set to the default when saving the
				form

	   Values:  A list of 'select' fields and the values that the fields
		    can have.  Each line has two words: the field name and
		    the values list, with individual values separated by
		    '/' (no spaces).

	   Presets: A list of fields and their default values, for fields
		    whose 'setting' flag is other than 'optional'.  Each
		    line has two words: the field name and the default
		    value.  If the value has spaces, it must be enclosed
		    in double quotes.  The following special defaults are
		    recognized:

			$user: the user entering the stream
			$now: the current date
			$blank: the words '<enter description here>'

	   Openable: A list of versioning options when editing a stream spec.
		    Each line has two words: the field name and the value.
		    Values are:
		        'none' : may not be changed in a stream edit
		        'isolate' : may be changed only in a stream edit
		        'propagate' : may be changed in a stream edit or integ


	   Comments: text to be included at the top of each stream spec,
		    to help the user fill out the form. Each line must begin
		    with the comment character '#'.

	The following field codes are reserved:

	    code 701, required: the stream name
	    code 705, optional: the date of last modification
	    code 706, optional: the date of creation
	    code 704, optional: the user who created the stream
	    code 703, required: a short title which may be updated
	    code 702, required: parent or 'none' if Type is mainline
	    code 708, required: one of mainline, release,
	        development(default), virtual, task
	    code 709, optional: a short description
	    code 707, optional:
	        allsubmit/ownersubmit,unlocked/locked
	        toparent/notoparent,fromparent/nofromparent"
	        mergedown/mergeany
	    code 710, required:
	        identify paths in the stream and how they are to be
	        generated in resulting clients of this stream
	        path types are: share/isolate/import/import+/import&/exclude
	    code 711, optional: remap a stream path in the resulting generated
	                        client view
	    code 712, optional: ignore a stream path in the resulting generated
	                        client view


	If you change the streamspec after streams have been created, note the
	following limitations:

	    Data is stored according to its code.  Fields can be renamed
	    by keeping the same code. Removing a code can cause the
	    associated data stored for the code to become abandoned.

	    Changing the definition of a code (for example, from 'text' to
	    'word') can require users to correct streams for the new format
	    during editing.

	The -o flag writes the stream template to the standard output.  The
	user's editor is not invoked.

	The -i flag reads a stream template from the standard input.  The
	user's editor is not invoked.

	'p4 streamspec' requires 'admin' access, which is granted by
	'p4 protect'.
