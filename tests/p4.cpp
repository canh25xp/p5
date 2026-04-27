/*
 * p4api.cc - a p4 client API example
 *
 * This file is part of the p4api distribution package.
 *
 * This barebones example simply mimics the regular p4 command line
 * program.
 *
 * Generally, compiling with the C++ compiler and linking with the
 * three provided libraries is sufficient to build this sample program.
 *
 * See the Perforce C/C++ API User's Guide for further information.
 * https://www.perforce.com/manuals/p4api/Content/P4API/Home-p4api.html
 *
 * $Id: //depot/r25.1/p4/api/p4api.cc#1 $
 */

#include <p4/clientapi.h>
#include <p4/p4libs.h>

int main(int argc, char **argv);
int main(int argc, char **argv) {
    ClientUser ui(1);
    ClientApi api;
    StrBuf msg;
    Error e;

    P4Libraries::Initialize(P4LIBRARIES_INIT_ALL, &e);

    if (e.Test()) {
        e.Fmt(&msg);
        fprintf(stderr, "%s\n", msg.Text());
        return 1;
    }

    // Any special protocol mods

    // api.SetProtocol( "tag" );

    // Enable client-side Extensions
    // api.EnableExtensions();

    // Connect to server

    api.Init(&e);

    if (e.Test()) {
        e.Fmt(&msg);
        fprintf(stderr, "%s\n", msg.Text());
        return 1;
    }

    // Run the command "argv[1] argv[2...]"

    api.SetArgv(argc - 2, argv + 2);
    api.Run(argv[1], &ui);

    // Close connection

    api.Final(&e);

    if (e.Test()) {
        e.Fmt(&msg);
        fprintf(stderr, "%s\n", msg.Text());
        return 1;
    }

    P4Libraries::Shutdown(P4LIBRARIES_INIT_ALL, &e);

    if (e.Test()) {
        e.Fmt(&msg);
        fprintf(stderr, "%s\n", msg.Text());
        return 1;
    }

    return 0;
}
