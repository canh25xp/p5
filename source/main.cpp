#include <CLI/CLI.hpp>
#include "log.h"

#include "commands.h"
#include "options.h"
#include "commands/changes.h"
#include "commands/clients.h"
#include "commands/users.h"
#include "commands/set.h"

#ifndef P5_APP_DESCRIPTION
#define P5_APP_DESCRIPTION "p5"
#endif

int main(int argc, char **argv) {
    // TODO: configuable at build time or run time.
    Log::ConciseLevel = 1;

    CLI::App app(P5_APP_DESCRIPTION);

    app.get_formatter()->right_column_width(999999);

    argv = app.ensure_utf8(argv);

    app.require_subcommand(0, 1);

    g_options.add(app);

    Commands commands;

    // --- File operations ---
    commands.add("add", "Open a new file to add it to the depot");
    commands.add("delete", "Open an existing file to delete it from the depot");
    commands.add("edit", "Open an existing file for edit");
    commands.add({"move", "rename"}, "Moves files from one location to another");
    commands.add("reopen", "Change the type or changelist number of an opened file");
    commands.add("revert", "Discard changes from an opened file");
    commands.add("lock", "Lock an opened file against changelist submission");
    commands.add("unlock", "Release a locked file but leave it open");

    // --- Changelist operations ---
    commands.add({"change", "changelist"}, "Create or edit a changelist description");
    commands.add(std::make_unique<Changes>());
    commands.add("describe", "Display a changelist description");
    commands.add("shelve", "Store files from a pending changelist into the depot");
    commands.add("unshelve", "Restore shelved files from a pending changelist");
    commands.add("reshelve", "Copy shelved files to a new or existing shelf");
    commands.add("submit", "Submit open files to the depot");
    commands.add("resolve", "Merge open files with other revisions or files");
    commands.add("resolved", "Show files that have been merged but not submitted");

    // --- Sync / workspace state ---
    commands.add("sync", "Synchronize the client with its view of the depot");
    commands.add("update", "Synonym for 'sync -s'");
    commands.add("flush", "Synonym for 'sync -k'");
    commands.add("clean", "Synonym for 'reconcile -w'");
    commands.add({"reconcile", "rec"}, "Reconcile client to offline workspace changes; 'rec' is a synonym for 'reconcile'");
    commands.add("status", "Synonym for 'reconcile -ead' (output uses local paths)");
    commands.add("have", "List revisions last synced");
    commands.add("opened", "Display list of files opened for pending changelist");
    commands.add("where", "Show how file names map through the client view");
    commands.add("ignores", "List P4IGNORE mappings");

    // --- Client / workspace ---
    commands.add({"client", "workspace"}, "Create or edit a client specification and its view");
    commands.add(std::make_unique<Clients>());

    // --- Depot / file info ---
    commands.add("files", "List files in the depot");
    commands.add("fstat", "Dump file info");
    commands.add("filelog", "List revision history of files");
    commands.add("annotate", "Print file lines along with their revisions");
    commands.add("diff", "Display diff of client file with depot file");
    commands.add("diff2", "Display diff of two depot files");
    commands.add("print", "Retrieve a depot file to the standard output");
    commands.add("dirs", "List subdirectories of a given depot directory");
    commands.add("sizes", "Display size information for files in the depot");
    commands.add("grep", "Print lines from text files matching a pattern");
    commands.add("cstat", "Dump change/sync status for current client");

    // --- Integration ---
    commands.add("integrate", "Schedule integration from one file to another");
    commands.add("integrated", "Show integrations that have been submitted");
    commands.add("interchanges", "Report changes that have not yet been integrated");
    commands.add("merge", "Schedule merge (integration) from one file to another");
    commands.add("copy", "Schedule copy of latest rev from one file to another");
    commands.add("undo", "Undo a range of revisions");

    // --- Branch / label / tag ---
    commands.add("branch", "Create or edit a branch specification");
    commands.add("branches", "Display list of branches");
    commands.add("label", "Create or edit a label specification and its view");
    commands.add("labels", "Display list of labels");
    commands.add("labelsync", "Synchronize label with the current client contents");
    commands.add("list", "Create an in-memory (label) list of depot files");
    commands.add("tag", "Tag files with a label");

    // --- Stream ---
    commands.add("stream", "Create or edit a stream specification");
    commands.add("streams", "Display list of streams");
    commands.add("streamlog", "List revision history of streams");
    commands.add("streamspec", "Edit the stream template");
    commands.add("switch", "Switch to a different stream, or create a new stream");
    commands.add("istat", "Show integrations needed for a stream");
    commands.add("populate", "Populate a branch or stream with files");
    commands.add("prune", "Remove unmodified branched files from a stream");

    // --- Job ---
    commands.add("job", "Create or edit a job (defect) specification");
    commands.add("jobs", "Display list of jobs");
    commands.add("fix", "Mark jobs as being fixed by named changelists");
    commands.add("fixes", "List what changelists fix what job");

    // --- User / group ---
    commands.add(std::make_unique<Users>());
    commands.add("user", "Create or edit a user specification");
    commands.add("group", "Change members of a user group");
    commands.add("groups", "List groups (of users)");
    commands.add("passwd", "Set the user's password on the server (and Windows client)");

    // --- Depot / repo admin ---
    commands.add("depot", "Create or edit a depot specification");
    commands.add("depots", "Display list of depots");
    commands.add("repo", "Create, edit or delete a repo specification");
    commands.add("repos", "Display a list of repos (depots of type graph)");

    // --- Counter / key ---
    commands.add("counter", "Display, set, or delete a counter");
    commands.add("counters", "Display list of known counters");
    commands.add("key", "Display, set, or delete a key/value pair");
    commands.add("keys", "Display list of known keys and their values");

    // --- Protection / review ---
    commands.add("protect", "Modify protections in the server namespace");
    commands.add("protects", "Display protections in place for a given user/path");
    commands.add("review", "List and track changelists (for the review daemon)");
    commands.add("reviews", "Show what users are subscribed to review files");
    commands.add("attribute", "Set per-revision attributes on revisions");

    // --- Auth ---
    commands.add("login", "Login to Perforce by obtaining a session ticket");
    commands.add("login2", "Perform a multi factor authentication");
    commands.add("logout", "Logout of Perforce by removing or invalidating a ticket");
    commands.add("tickets", "Display list of session tickets for this user");
    commands.add("trust", "Establish trust of an SSL connection");

    // --- Info / logging ---
    commands.add("info", "Print out client/server information");
    commands.add("logger", "Report what jobs and changelists have changed");
    commands.add("aliases", "Display the content of the P4ALIASES file"); // TODO: This does not support by P4API, client must implement
    commands.add("help", "Print the requested help message");

    commands.add(std::make_unique<Set>(g_options));

    commands.install(app);

    try {
        app.parse(argc, argv);
    } catch (const CLI::ParseError &e) {
        // TODO: Use CLI_ERROR for logging
        return app.exit(e);
    }
    return 0;
}
