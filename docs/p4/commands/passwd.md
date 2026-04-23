# p4 passwd


    passwd -- Set the user's password on the server (and Windows client)

    p4 passwd [-O oldPassword -P newPassword] [user]

	'p4 passwd' sets the user's password on the server.

	After a password is set for a user, the same password must be set on
	the client in the environment variable $P4PASSWD to enable the user
	to use all Perforce client applications on that machine. (On Windows,
	you can use 'p4 passwd' to configure the password in the environment.)

	'p4 passwd' prompts for both the old password and the new password
	with character echoing turned off.  To delete the password, set it to
	an empty string.

	The -O flag provides the old password, avoiding prompting. If you
	specify -O, you must also specify -P.

	The -P flag provides the new password, avoiding prompting.

	If you are using ticket-based authentication, changing your password
	automatically invalidates all of your tickets and logs you out.

	Specifying a username as an argument to 'p4 passwd' requires 'super'
	access granted by 'p4 protect'.
