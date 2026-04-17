## p5 set

`p5 set --all`

Not part of `p4 set`. Lists every `P4*` environment variable name the
linked Helix client API treats as known (`Enviro::IsKnown`), using the
same per-variable formatting as `p5 set` with no arguments when the
client has resolved a value (current value and where it came from).
Names with no value yet are still listed as `P4FOO=` so you can use the
output as a checklist against `p4 help environment`.
