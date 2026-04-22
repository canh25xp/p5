#include "CLI/CLI.hpp"

#include "commands.h"
#include "options.h"
#include "command_runners.h"
#include "commands/set.h"

#ifndef P5_APP_DESCRIPTION
#define P5_APP_DESCRIPTION "p5"
#endif

int main(int argc, char **argv) {
    CLI::App app(P5_APP_DESCRIPTION);

    argv = app.ensure_utf8(argv);

    g_options.add(app);

    Commands commands;

    // --- File operations ---
    commands.add_passthrough("add", "Open a new file to add it to the depot");
    commands.add_passthrough("delete", "Open an existing file to delete it from the depot");
    commands.add_passthrough("edit", "Open an existing file for edit");
    commands.add_passthrough("move", "Moves files from one location to another", {"rename"});
    commands.add_passthrough("reopen", "Change the type or changelist number of an opened file");
    commands.add_passthrough("revert", "Discard changes from an opened file");
    commands.add_passthrough("lock", "Lock an opened file against changelist submission");
    commands.add_passthrough("unlock", "Release a locked file but leave it open");

    // --- Changelist operations ---
    commands.add_passthrough("change", "Create or edit a changelist description", {"changelist"});
    commands.add_passthrough("changes", "Display list of pending and submitted changelists", {"changelists"});
    commands.add_passthrough("describe", "Display a changelist description");
    commands.add_passthrough("shelve", "Store files from a pending changelist into the depot");
    commands.add_passthrough("unshelve", "Restore shelved files from a pending changelist");
    commands.add_passthrough("reshelve", "Copy shelved files to a new or existing shelf");
    commands.add_passthrough("submit", "Submit open files to the depot");
    commands.add_passthrough("resolve", "Merge open files with other revisions or files");
    commands.add_passthrough("resolved", "Show files that have been merged but not submitted");

    // --- Sync / workspace state ---
    commands.add_passthrough("sync", "Synchronize the client with its view of the depot");
    commands.add_passthrough("update", "Update the client with its view of the depot");
    commands.add_passthrough("flush", "Fake a 'p4 sync' by not moving files");
    commands.add_passthrough("clean", "Delete or refresh local files to match depot state");
    commands.add_passthrough("reconcile", "Reconcile client to offline workspace changes", {"rec"});
    commands.add_passthrough("status", "Preview reconcile of client to offline workspace changes");
    commands.add_passthrough("have", "List revisions last synced");
    commands.add_passthrough("opened", "Display list of files opened for pending changelist");
    commands.add_passthrough("where", "Show how file names map through the client view");
    commands.add_passthrough("ignores", "List P4IGNORE mappings");

    // --- Client / workspace ---
    commands.add_passthrough("client", "Create or edit a client specification and its view", {"workspace"});
    commands.add_passthrough("clients", "Display list of known clients", {"workspaces"});

    // --- Depot / file info ---
    commands.add_passthrough("files", "List files in the depot");
    commands.add_passthrough("fstat", "Dump file info");
    commands.add_passthrough("filelog", "List revision history of files");
    commands.add_passthrough("annotate", "Print file lines along with their revisions");
    commands.add_passthrough("diff", "Display diff of client file with depot file");
    commands.add_passthrough("diff2", "Display diff of two depot files");
    commands.add_passthrough("print", "Retrieve a depot file to the standard output");
    commands.add_passthrough("dirs", "List subdirectories of a given depot directory");
    commands.add_passthrough("sizes", "Display size information for files in the depot");
    commands.add_passthrough("grep", "Print lines from text files matching a pattern");
    commands.add_passthrough("cstat", "Dump change/sync status for current client");

    // --- Integration ---
    commands.add_passthrough("integrate", "Schedule integration from one file to another");
    commands.add_passthrough("integrated", "Show integrations that have been submitted");
    commands.add_passthrough("interchanges", "Report changes that have not yet been integrated");
    commands.add_passthrough("merge", "Schedule merge (integration) from one file to another");
    commands.add_passthrough("copy", "Schedule copy of latest rev from one file to another");
    commands.add_passthrough("undo", "Undo a range of revisions");

    // --- Branch / label / tag ---
    commands.add_passthrough("branch", "Create or edit a branch specification");
    commands.add_passthrough("branches", "Display list of branches");
    commands.add_passthrough("label", "Create or edit a label specification and its view");
    commands.add_passthrough("labels", "Display list of labels");
    commands.add_passthrough("labelsync", "Synchronize label with the current client contents");
    commands.add_passthrough("list", "Create an in-memory (label) list of depot files");
    commands.add_passthrough("tag", "Tag files with a label");

    // --- Stream ---
    commands.add_passthrough("stream", "Create or edit a stream specification");
    commands.add_passthrough("streams", "Display list of streams");
    commands.add_passthrough("streamlog", "List revision history of streams");
    commands.add_passthrough("streamspec", "Edit the stream template");
    commands.add_passthrough("switch", "Switch to a different stream, or create a new stream");
    commands.add_passthrough("istat", "Show integrations needed for a stream");
    commands.add_passthrough("populate", "Populate a branch or stream with files");
    commands.add_passthrough("prune", "Remove unmodified branched files from a stream");

    // --- Job ---
    commands.add_passthrough("job", "Create or edit a job (defect) specification");
    commands.add_passthrough("jobs", "Display list of jobs");
    commands.add_passthrough("fix", "Mark jobs as being fixed by named changelists");
    commands.add_passthrough("fixes", "List what changelists fix what job");

    // --- User / group ---
    commands.add("users", "List Perforce users (p5 formatted output)", run_users);
    commands.add_passthrough("user", "Create or edit a user specification");
    commands.add_passthrough("group", "Change members of a user group");
    commands.add_passthrough("groups", "List groups (of users)");
    commands.add_passthrough("passwd", "Set the user's password on the server (and Windows client)");

    // --- Depot / repo admin ---
    commands.add_passthrough("depot", "Create or edit a depot specification");
    commands.add_passthrough("depots", "Display list of depots");
    commands.add_passthrough("repo", "Create, edit or delete a repo specification");
    commands.add_passthrough("repos", "Display a list of repos (depots of type graph)");

    // --- Counter / key ---
    commands.add_passthrough("counter", "Display, set, or delete a counter");
    commands.add_passthrough("counters", "Display list of known counters");
    commands.add_passthrough("key", "Display, set, or delete a key/value pair");
    commands.add_passthrough("keys", "Display list of known keys and their values");

    // --- Protection / review ---
    commands.add_passthrough("protect", "Modify protections in the server namespace");
    commands.add_passthrough("protects", "Display protections in place for a given user/path");
    commands.add_passthrough("review", "List and track changelists (for the review daemon)");
    commands.add_passthrough("reviews", "Show what users are subscribed to review files");
    commands.add_passthrough("attribute", "Set per-revision attributes on revisions");

    // --- Auth ---
    commands.add_passthrough("login", "Login to Perforce by obtaining a session ticket");
    commands.add_passthrough("login2", "Perform a multi factor authentication");
    commands.add_passthrough("logout", "Logout of Perforce by removing or invalidating a ticket");
    commands.add_passthrough("tickets", "Display list of session tickets for this user");
    commands.add_passthrough("trust", "Establish trust of an SSL connection");

    // --- Info / logging ---
    commands.add_passthrough("info", "Print out client/server information");
    commands.add_passthrough("logger", "Report what jobs and changelists have changed");
    commands.add_passthrough("aliases", "Display the content of the P4ALIASES file");
    commands.add_passthrough("help", "Print the requested help message");

    commands.install(app);
    register_set(app, g_options);

    CLI11_PARSE(app, argc, argv);

    return 0;
}
