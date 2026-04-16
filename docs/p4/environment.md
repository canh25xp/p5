# Environment variables used by Perforce:

    Variable         Defines                         More information
    --------         -------                         ------------------------
    P4ALIASES        Name of aliases file            p4 help aliases
    P4ALTSYNC        Alternative sync agent          P4 Command Reference
    P4CHARSET        Client's local character set    p4 help charset
    P4COMMANDCHARSET Client's local character set
                     (for command line operations)   p4 help charset
    P4CLIENT         Name of client workspace        p4 help client
    P4CLIENTPATH     Directories client can access   P4 Command Reference
    P4CONFIG         Name of configuration file      P4 Command Reference
    P4DIFF           Diff program to use on client   p4 help diff
    P4DIFFUNICODE    Diff program to use on client   p4 help diff
    P4EDITOR         Editor invoked by p4 commands   p4 help change, etc
    P4ENVIRO         Name of environment file        P4 Command Reference
    P4EXTENSIONS     Name of client Extension file   P4 Command Reference
    P4HOST           Name of host computer           p4 help usage
    P4IGNORE         Name of ignore file             P4 Command Reference
    P4LANGUAGE       Locale for server messages      p4 help usage
    P4LOGINSSO       Client side credentials script  p4 help triggers
    P4MERGE          Merge program to use on client  p4 help resolve
    P4MERGEUNICODE   Merge program to use on client  p4 help resolve
    P4PAGER          Pager for 'p4 resolve' output   p4 help resolve
    P4PASSWD         User password passed to server  p4 help passwd
    P4PORT           Port to which client connects   p4 help info
    P4SSLDIR         SSL server credential directory P4 Command Reference
    P4TICKETS        Location of tickets file        P4 Command Reference
    P4TRUST          Location of SSL trust file      P4 Command Reference
    P4USER           Perforce user name              p4 help usage
    PWD              Current working directory       p4 help usage
    TMP, TEMP        Directory for temporary files   P4 Command Reference

    For details about configuring Windows settings, issue the 'p4 help set'
    command.  The syntax for setting an environment variable depends on the
    OS/shell.  Many shells permit you to set shell variables separately from
    environment variables, but Perforce cannot access the shell variable,
    only the environment variable.

    Variables of primary interest to typical users are:
    $P4CLIENT, $P4PORT and $P4PASSWD.

    Environment variables used by the Perforce server:

    Variable        Defines                          More information
    --------        -------                          ------------------------
    P4AUDIT         Name of server audit file        p4d -h
    P4AUTH          Authentication server address    P4 Command Reference
    P4CHANGE        Global changelist server address P4 Command Reference
    P4DEBUG         Server diagnostic trace flags    P4 Command Reference
    P4DESCRIPTION   Text description for server      P4 Command Reference
    P4JOURNAL       Name of server journal file      p4d -h
    P4LOG           Name of server log file          p4d -h
    P4NAME          Unique server name               P4 Command Reference
    P4PORT          Port on which server listens     p4d -h
    P4ROOT          Server root directory            p4d -h
    P4PROOT         Proxy Server metadata directory  p4p -h
    P4PCACHE        Proxy Server archives directory  p4p -h
    P4SSLDIR        Server SSL credentials directory P4 Command Reference
    P4TARGET        Target server for proxy/replica  P4 Command Reference
    P4TICKETS       Location of tickets file         P4 Command Reference
    P4TRUST         Location of SSL trust file       P4 Command Reference
    TMP, TEMP       Directory for temporary files    P4 Command Reference

    To set server environment variables persistently, use the 'p4 configure'
    command.  Note that P4ROOT and TMP/TEMP cannot be set using 'p4 configure'.
    Also note that server environment variables are not searched for in
    configuration or environment files, i.e. P4CONFIG and P4ENVIRO are ignored.
