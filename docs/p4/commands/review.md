# p4 review


    review -- List and track changelists (for the review daemon)

    p4 review [-c changelist#] [-t counter]

	'p4 review' lists changelists that have not been reviewed before,
	as tracked by the specified counter.  If the counter is omitted,
	'p4 review' lists all changelists.  (If a changelist# and counter
	are specified, 'p4 review' sets the counter to that changelist# and
	produces no output.  This functionality has been superceded by the
	'p4 counter' command.)

	'p4 review' is not meant as an end-user command.  It exists to
	support an automated change review daemon.
