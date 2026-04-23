# p4 logger


    logger -- Report changed jobs and changelists

    p4 logger [-c sequence#] [-t counter]

	Dumps the event log, which notes updates to changes and jobs, for
	use with defect tracking integration.  The event log is enabled
	by setting the counter 'logger' to 0 using the 'p4 counter' command.
	Each event is assigned a sequence number.  The presence of an entry
	in the log doesn't guarantee that the named entity has changed.

	If a sequence# is specified using the -c flag, only events after that
	number are listed. If a counter is given using the -t flag, only events
	after the number of that counter are listed.  If both are specified,
	the counter is updated to the sequence number and nothing is output.
	If the update brings the counter to the highest sequence number in
	the log, the log is cleared.

	Important: 'p4 logger' is not intended for end users. It is used by
	P4DTG to help propagate information to an external defect tracking
	system.

	'p4 logger -c' requires 'review' access, which is granted by 'p4
	protect'.

	In a distributed installation, 'p4 logger' commands should be issued
	to the Commit Server, not to an Edge Server.
