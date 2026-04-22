# TODO

- [ ] Drop-in replaceable implementation for `p4`:
  - [x] All `p4` subcommands (See `p4 help commands`)
  - [/] All `p4` global arguments (See `p4 help usage`)
  - [/] All `p4` environment variables and configuration files(See `p4 help environment`)
- [ ] Plan Improvements:
  - [ ] Add Feature:
    - [ ] Workspace awareness: `p5` should be able to set P4CLIENT based on current directory
    - [ ] Convenience way to add and remove workspace mapping
    - [ ] Helix Swarm integration
    - [ ] Code Grok integration to replace `p4 grep`
  - [ ] Improve Feature:
    - [ ] `p4 blame` filter by lines
    - [ ] `p4 login` remember password and auto login
  - [ ] Improve Visual
    - [ ] Output coloring
    - [ ] Diff coloring
    - [ ] Output presentation: Some command's output is long and redundant (e.g. `p4 where`)
