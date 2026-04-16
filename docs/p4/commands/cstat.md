# p4 cstat


    cstat -- Dump change/sync status for current client

    p4 cstat [files...]

	Lists changes that are needed, had or partially synced in the current
	client. The output is returned in tagged format, similar to the fstat
	command.

	The fields that cstat displays are:

		change   changelist number
		status   'have', 'need' or 'partial'
