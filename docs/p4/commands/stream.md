# p4 stream


    stream -- Create, delete, or modify a stream specification

    p4 stream [-f] [-d] [-P parent] [-t type]
              [--parentview inherit|noinherit] [name]
    p4 stream [-o [-v]] [-P parent] [-t type]
              [--parentview inherit|noinherit] [name[@change]]
    p4 stream --obliterate [-y] name[@change]
    p4 stream [-f] [-d] name
    p4 stream -i [-f]
    p4 stream edit
    p4 stream parentview [-c changelist# -n -o --source-comments]
                         {--inherit|--noinherit}
    p4 stream resolve [-a<flag>] [-n] [-o]
    p4 stream revert

	A stream specification ('spec') names a path in a stream depot to be
	treated as a stream.  (See 'p4 help streamintro'.)  The spec also
	defines the stream's lineage, its view, and its expected flow of
	change.

	The 'p4 stream' command puts the stream spec into a temporary file and
	invokes the editor configured by the environment variable $P4EDITOR.
	When creating a stream, the type of the stream must be specified with
	the '-t' flag.  Saving the file creates or modifies the stream spec.

	By default updates to the stream spec are automatically visible to
	all clients; see 'p4 help streamcmds' for information on 'opening' a
	stream spec to stage updates on one client before submitting them.

	Creating a stream spec does not branch a new stream.  To branch a
	stream, use 'p4 copy -r -S stream', where 'stream' is the name of a
	stream spec.

	The stream spec contains the following fields:

	Stream:   The stream's path in a stream depot, of the form
	          //depotname/streamname. This is both the name of the stream
	          spec and the permanent, unique identifier of the stream.
	          This stream path may extend beyond the traditional form by
	          updating the 'StreamDepth' field in the stream depot when
	          creating a new stream depot.  Here, depth relates to the
	          number of single slashes in stream names following the
	          depot name (so traditional form above reflects StreamDepth
	          of 1.  All streams within a stream depot must adhere to
	          this same format.

	Update:   The date this stream spec was last changed.

	Access:   The date of the last command used with this spec.

	Owner:    The stream's owner. A stream can be owned by a user, or
	          owned by a group. Can be changed.

	Name:     An alternate name of the stream, for use in display outputs.
	          Defaults to the final path in the 'streamname' portion in
	          the stream. Can be changed.

	Parent:   The parent of this stream. Can be 'none' if the stream type
	          is 'mainline',  otherwise must be set to an existing stream
	          identifier, of the form //depotname/streamname.
	          Can be changed.

	Type:     'mainline', 'virtual', 'development', 'release' or 'task'.
	          Defines the role of a stream: A 'mainline' may not have a
	          parent. A 'virtual' stream is not a stream but an alternate
	          view of its parent stream.  The 'development' and 'release'
	          streams have controlled flow. Can be changed.  A 'task'
	          stream is a lightweight short-lived stream that only
	          promotes edited files to the repository; branched and
	          integrated files are stored in shadow tables that are
	          removed when the task stream is deleted or unloaded.

	          Flow control is provided by 'p4 copy -S' and 'p4 merge -S'.
	          These commands restrict the flow of change as follows:

	          Stream Type   Direction of flow     Allowed with
	          -----------   -----------------     ------------
	          development   to parent stream      'p4 copy'
	          task          to parent stream      'p4 copy'
	          release       to parent stream      'p4 merge'
	          development   from parent stream    'p4 merge'
	          release       from parent stream    'p4 copy'

	Description: An optional description of the stream.

	Options:  Flags to configure stream behavior. Defaults are marked *:

	          unlocked *      Indicates whether the stream spec is locked
	          locked          against modifications. If locked, the spec
	                          may not be deleted, and only its owner or
	                          group users can modify it.

	          allsubmit *     Indicates whether all users or only the
	          ownersubmit     owner (or group users) of the stream may
	                          submit changes to the stream path.

	          toparent *      Indicates if controlled flow from the
	          notoparent      stream to its parent is expected to occur.

	          fromparent *    Indicates if controlled flow to the stream
	          nofromparent    from its parent is expected to occur.

	          mergedown *     Indicates if merge flow is restricted or
	          mergeany        merge is permitted from any other stream.

	          The [no]fromparent and [no]toparent options determine if
	          'p4 copy -S' and 'p4 merge -S' allow change to flow between
	          a stream and its parent. A 'virtual' stream must have its
	          flow options set as 'notoparent' and 'nofromparent'. Flow
	          options are ignored for 'mainline' streams.

	ParentView: 'inherit' or 'noinherit'.  Defines whether the stream
	          inherits a view from its parent.

	          If a ParentView is inherit, the Paths, Remapped, and Ignored
	          fields will be affected.  The view created from each field is
	          composed of the stream's fields and the set of fields
	          "inherited" from each of the stream's ancestors. The
	          inheritance is implicit, so the inherited Paths, Remapped,
	          and Ignored values will not be displayed with the current
	          stream specification.

	          If a ParentView is noinherit, the Paths, Remapped, and
	          Ignored fields are not affected by the stream's ancestors.
	          The child views are exactly what is specified in the Paths,
	          Remapped, and Ignored fields.

	          Task streams and virtual streams may only have inherit
	          ParentViews. All other stream types may have inherit or
	          noinherit ParentViews.

	          When a development, release, or mainline stream is first
	          created, the ParentView field may be set with the
	          '--parentview' parameter. The parameter may be set with
	          values 'inherit' or 'noinherit'.

	          When creating a development, release, or mainline stream
	          without the '--parentview' parameter, the configurable
	          dm.stream.parentview will determine the default value for
	          the ParentView field.  The configurable has the following
	          values:

	              0: all new streams have an inherit ParentView.
	              1: all new mainline, development and release streams have
	                 a noinherit ParentView, all new task and virtual
	                 streams have an inherit ParentView.
	              2: all new release streams have a noinherit ParentView,
	                 all new non-release streams have an inherit ParentView
	                 value.

	          The default value for dm.stream.parentview is 0.

	          After a stream has been created, the ParentView field cannot
	          be changed by editing the stream specification.  Instead, use
	          the command 'p4 stream parentview' to change stream's
	          parentview. See 'p4 help streamcmds' for more details.


	Components:  One or more lines that include other streams' views within
	          the current stream.  Each line is of the form:

	              <component_type> <component_folder> <stream>[@change]
	
	          where

	              <component_type> is a single keyword.

	              <component_folder> a directory path relative to the
	              root of the client workspace.

	              <stream> is the stream path and name of an existing
	              stream.  An optional '@change' or '@automatic_label' can
	              be specified to get the streamviews at or before the
	              given change or automatic label.

	          For example:

	              readonly dirA //stream/mainA@37

	          A component is a stream that contributes its views to the
	          the stream that defines the component. A stream that defines
	          one or more other streams as components is called a consuming
	          stream.

	          If a component is defined to be at a given change, that
	          component's views come from the last time the stream was
	          edited, either at the specified change or earlier. Any file
	          sync'd from a component@change's views will be at a revision
	          at or before the specified change.

	          The component types are:

	              readonly -- all views that originate from the component
	                  are readonly;  files can be sync'd but not edited and
	                  submitted.  Note that if the component has an import+
	                  path, that path is readonly for the consuming stream.

	              writeimport+ -- all views that originate from import+
	                  paths in the component can be opened for edit and
	                  submitted (writable);  all other views that
	                  originate from the component are readonly.

	              writeall -- all views that originate from share, isolate,
	                  or import+ paths in the component can be opened for
	                  edit and submitted (writable);  all other views that
	                  originate from the component are readonly.

	          Note that component stream views will be included when the
	          consuming stream is itself defined as a component for some
	          other stream.  For example:

	              Suppose there are streams //stream/mainA, //stream/mainB,
	              and main //stream/mainC.

	              Suppose that //stream/mainC defines a Path

	                  import libXYZ/lib1.a //libs/libXYZ/lib1.a

	              Then //stream/mainB defines a component

	                  readonly dirC //stream/mainC

	              //stream/mainA defines a component

	                  readonly dirB //stream/mainB

	              Then a workspace ws, which is associated with
	              //stream/mainA, will have views

	                  //stream/mainA/... //ws/...
	                  //stream/mainB/... //ws/dirB/...
	                  //stream/mainC/... //ws/dirB/dirC/...
	                  //libs/libXYZ/lib1.a //ws/dirB/dirC/libXYZ/lib1.a

	          Note that in a chain of components of different types, the
	          most restrictive type becomes the effective component type
	          for views originating from the end component in the chain.

	          For example, if component B is defined as writeall in A, and
	          B has a readonly component C, then the views coming from
	          paths in C are readonly.  Similarly, if B is defined as
	          writeimport+ in A and C is defined as writeall in B, then the
	          views in A which originate from share or isolate paths in C
	          are readonly, while import+ paths in C are writable.

	          The configurable, dm.stream.components, can be used to enable
	          or disable the inclusion of stream components in the
	          workspace view.  Use values

	              0 (default): disables stream components in the workspace
	                  view.

	              1: enables stream components in the workspace view.


	Paths:    One or more lines that define file paths in the stream view.
	          Each line is of the form:

	              <path_type> <view_path> [<depot_path>]

	          where <path_type> is a single keyword, <view_path> is a file
	          path with no leading slashes, and the optional <depot_path>
	          is a file path beginning with '//'.  Both <view_path> and
	          <depot_path> may contain trailing wildcards, but no leading
	          wildcards.  Embedded wildcards are restricted to the final
	          expression of the path, with no further slashes.
	          Lines in the Paths field may appear in any order.
	          A duplicated <view_path> overrides its preceding entry.

	          For example:

	              share   src/...
	              import  lib/abc/...  //over/there/abc/...
	              import  module2/*.txt //demo3/main/module2/*.txt
	              isolate bin/*
	              isolate bin/...dll

	          Default is:

	              share   ...

	          The <path_type> keyword must be one of:

	          share:  <view_path> will be included in client views and
	                  in branch views. Files in this path are accessible
	                  to workspaces, can be submitted to the stream, and
	                  can be integrated with the parent stream.

	          isolate: <view_path> will be included in client views but
	                   not in branch views. Files in this path are
	                   accessible to workspaces, can be submitted to the
	                   stream, but are not integratable with the parent
	                   stream.

	          import: <view_path> will be included in client views but
	                  not in branch views. Files in this path are mapped
	                  as in the parent stream's view (the default) or to
	                  <depot_path> (optional); they are accessible to
	                  workspaces, but can not be submitted or integrated
	                  to the stream.  If <depot_path> is used it may
	                  include a changelist or automatic label specifier;
	                  clients of that stream will be limited to seeing
	                  revisions at that point in time or lower within that
	                  depot path (See the explanation of 'ChangeView' in
	                  'p4 help client' for more details)

	          import+: <view_path> same as 'import' except that files can
	                   be submitted to the import path.  Note that an
	                   import+ path that references a path in a stream
	                   depot will not respect the views or flow rules of
	                   any other streams that depend on that depot path,
	                   so undesirable interactions are possible.

	          import&: <view_path> <depot_path> same as 'import' except
	                   that multiple import& paths can map the
	                   same <depot_path> to multiple <view_path>s.
	                   Files marked this way are readonly.
	                   <depot_path> is required.

	          exclude: <view_path> will be excluded from client views
	                   and branch views. Files in this path are not
	                   accessible to workspaces, and can't be submitted
	                   or integrated to the stream.

	          Paths are inherited by child stream views. A child stream's
	          paths can downgrade the inherited view, but not upgrade it.
	          (For instance, a child stream can downgrade a shared path to
	          an isolated path, but it can't upgrade an isolated path to a
	          shared path.) Note that <depot_path> is relevant only when
	          <path_type> is 'import'.

	Remapped: Optional; one or more lines that define how stream view paths
	          are to be remapped in client views.  Each line is of the
	          form:

	              <view_path_1> <view_path_2>

	          where <view_path_1> and <view_path_2> are Perforce view paths
	          with no leading slashes, no leading wildcards, or wildcards
	          embedded between slashes:
	          For example:

	              ...    x/...
	              y/*    y/z/*

	          Line ordering in the Remapped field is significant; if more
	          than one line remaps the same files, the later line has
	          precedence.  Remapping is inherited by child stream client
	          views.

	Ignored: Optional; a list of file or directory names to be recursively
	         excluded in branch and client views. For example:

	             /tmp      # ignores files named 'tmp'
	             /tmp/...  # ignores dirs named 'tmp'
	             .tmp      # ignores file names ending in '.tmp'

	         Lines in the Ignored field may appear in any order.  Ignored
	         names are inherited by child stream client views.  Note that
	         due to their recursive nature, large numbers of Ignored
	         entries may be inefficient; consider P4IGNORE as another way
	         to prevent specific filename patterns from being added.

	The -d flag causes the stream spec to be deleted.  A stream spec may
	not be deleted if it is referenced by child streams or stream clients.
	Deleting a stream spec does not remove stream files, but it does mean
	changes can no longer be submitted to the stream's path.
	The stream spec is marked deleted while preserving its metadata.
	Report on deleted streams using 'p4 stream -o deletedStream@change',
	'p4 streams -a', and 'p4 streamlog deletedStream'.
	( See --obliterate. )

	The --obliterate flag deletes the stream from all metadata,
	removes the stream from pending and committed change lists, and allows
	deletion of such changelists. Obliterate can be restricted to
	a changelist: p4 stream --obliterate name@change.
	By default, --obliterate displays a preview of the
	results. To execute the operation, specify the -y flag.
	--obliterate requires 'admin' access granted by 'p4 protect'.

	The -o flag causes the stream spec to be written to the standard
	output. The user's editor is not invoked. -v may be used with -o to
	expose the automatically generated client view for this stream.
	('p4 help branch' describes how to expose the branch view.)
	If an @changelist specifier is included with the stream name, the
	(submitted) stream as of that change is displayed rather than the
	latest (or open) version.

	The -P flag can be used to insert a value into the Parent field of a
	new stream spec. The flag has no effect on an existing spec.

	The -t flag is used to insert a value into the type field of a
	new stream spec and to adjust the default fromparent option
	for a new 'release' -type stream. The flag has no effect on an
	existing spec.

	The -i flag causes a stream spec to be read from the standard input.
	The user's editor is not invoked.

	The -f flag allows a user other than the owner to modify or delete a
	locked stream. It requires 'admin' access granted by 'p4 protect'.

	See 'p4 help streamcmds' for information on the 'p4 stream edit',
	'p4 stream resolve', 'p4 stream revert', and 'p4 stream parentview'
	commands.
