# p5

> Complementary p4 command-line tools

`p5` is a command-line tool meant to replace the `p4` CLI.
It adds features `p4` lacks and improves existing behavior, visually and functionally.
The interface is designed to match the existing `p4` tool as closely as possible so users can easily migrate to `p5`.

See [Changes](#changes) for how `p5` is differ from `p4`.

## Usage

```
p5: Complementary p4 command-line tools

[GLOBAL OPTIONS...] <subcommand> [arguments...]


OPTIONS:
  -h,     --help              Print this help message and exit

Global options:
  -v, -V, --version           Display program version information and exit
  -u,     --user TEXT (Env:P4USER)
                              Override P4USER (default: $P4USER)
  -p,     --port TEXT (Env:P4PORT)
                              Override P4PORT (default: $P4PORT)
  -c,     --client TEXT (Env:P4CLIENT)
                              Override P4CLIENT (default: $P4CLIENT)
  -z TEXT ...                 Set Helix protocol variable (name or name=value); repeat for multiple

SUBCOMMANDS:
  add                         Open a new file to add it to the depot
  delete                      Open an existing file to delete it from the depot
  edit                        Open an existing file for edit
  move, rename                Moves files from one location to another
  reopen                      Change the type or changelist number of an opened file
  revert                      Discard changes from an opened file
  lock                        Lock an opened file against changelist submission
  unlock                      Release a locked file but leave it open
  change, changelist          Create or edit a changelist description
  changes, changelists        Display list of pending and submitted changelists
  describe                    Display a changelist description
  shelve                      Store files from a pending changelist into the depot
  unshelve                    Restore shelved files from a pending changelist
  reshelve                    Copy shelved files to a new or existing shelf
  submit                      Submit open files to the depot
  resolve                     Merge open files with other revisions or files
  resolved                    Show files that have been merged but not submitted
  sync                        Synchronize the client with its view of the depot
  update                      Synonym for 'sync -s'
  flush                       Synonym for 'sync -k'
  clean                       Synonym for 'reconcile -w'
  reconcile, rec              Reconcile client to offline workspace changes; 'rec' is a synonym for 'reconcile'
  status                      Synonym for 'reconcile -ead' (output uses local paths)
  have                        List revisions last synced
  opened                      Display list of files opened for pending changelist
  where                       Show how file names map through the client view
  ignores                     List P4IGNORE mappings
  client, workspace           Create or edit a client specification and its view
  clients, workspaces         Display list of known clients
  files                       List files in the depot
  fstat                       Dump file info
  filelog                     List revision history of files
  annotate                    Print file lines along with their revisions
  diff                        Display diff of client file with depot file
  diff2                       Display diff of two depot files
  print                       Retrieve a depot file to the standard output
  dirs                        List subdirectories of a given depot directory
  sizes                       Display size information for files in the depot
  grep                        Print lines from text files matching a pattern
  cstat                       Dump change/sync status for current client
  integrate                   Schedule integration from one file to another
  integrated                  Show integrations that have been submitted
  interchanges                Report changes that have not yet been integrated
  merge                       Schedule merge (integration) from one file to another
  copy                        Schedule copy of latest rev from one file to another
  undo                        Undo a range of revisions
  branch                      Create or edit a branch specification
  branches                    Display list of branches
  label                       Create or edit a label specification and its view
  labels                      Display list of labels
  labelsync                   Synchronize label with the current client contents
  list                        Create an in-memory (label) list of depot files
  tag                         Tag files with a label
  stream                      Create or edit a stream specification
  streams                     Display list of streams
  streamlog                   List revision history of streams
  streamspec                  Edit the stream template
  switch                      Switch to a different stream, or create a new stream
  istat                       Show integrations needed for a stream
  populate                    Populate a branch or stream with files
  prune                       Remove unmodified branched files from a stream
  job                         Create or edit a job (defect) specification
  jobs                        Display list of jobs
  fix                         Mark jobs as being fixed by named changelists
  fixes                       List what changelists fix what job
  users, u                    List Perforce users (p5 formatted output)
  user                        Create or edit a user specification
  group                       Change members of a user group
  groups                      List groups (of users)
  passwd                      Set the user's password on the server (and Windows client)
  depot                       Create or edit a depot specification
  depots                      Display list of depots
  repo                        Create, edit or delete a repo specification
  repos                       Display a list of repos (depots of type graph)
  counter                     Display, set, or delete a counter
  counters                    Display list of known counters
  key                         Display, set, or delete a key/value pair
  keys                        Display list of known keys and their values
  protect                     Modify protections in the server namespace
  protects                    Display protections in place for a given user/path
  review                      List and track changelists (for the review daemon)
  reviews                     Show what users are subscribed to review files
  attribute                   Set per-revision attributes on revisions
  login                       Login to Perforce by obtaining a session ticket
  login2                      Perform a multi factor authentication
  logout                      Logout of Perforce by removing or invalidating a ticket
  tickets                     Display list of session tickets for this user
  trust                       Establish trust of an SSL connection
  info                        Print out client/server information
  logger                      Report what jobs and changelists have changed
  aliases                     Display the content of the P4ALIASES file
  help                        Print the requested help message
  set                         Set or display Perforce variables
```

## Changes

<!-- TODO: Add changes compare to `p4` -->
