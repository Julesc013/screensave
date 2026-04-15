# Plasma U07 Proof Result Taxonomy

## Purpose

This note freezes the U07 result language used by reports, captures, and later
decision work.

## Result Classes

- `validated`: backed by real smoke, capture, or generated audit evidence in
  the current repo
- `partial`: implemented and exercised for a bounded subset or by a weaker but
  still honest proof method
- `documented_only`: recorded in docs or source but not backed here by direct
  generated evidence
- `unsupported`: explicitly outside the current admitted or first-class surface
- `blocked`: would require environments or capture surfaces that the current
  repo does not have

## Proof Methods Referenced By U07

- `render_signature`: deterministic smoke treated or presented buffer
  difference
- `structural_plan`: resolved settings, selection, and compiled-plan truth
  without claiming screenshot proof
- `benchlab_capture`: requested, resolved, and degraded report proof on the
  BenchLab text surface
- `palette_distance`: compiled theme-palette separation proof
- `preset_signature_distance`: compiled preset-signature difference proof
- `smoke_matrix_compile`: exhaustive stable matrix compile and validation
  coverage

## How To Use These Classes

Use these classes conservatively.

- a row is not `validated` unless the current phase generated direct evidence
- `partial` is acceptable when the surface is real but only honestly provable by
  structural or bounded capture methods
- `documented_only` must never be treated as proof
- `unsupported` must be read as a non-claim, not as a silent degrade
- `blocked` means the repo needs an environment or harness it does not yet have

## How Later Phases Should Interpret Them

`U08` and later phases should treat:

- `validated` as evidence-backed proof substrate
- `partial` as bounded input that still needs careful QA interpretation
- `documented_only` as non-proof doctrine or source truth
- `unsupported` as an admitted boundary
- `blocked` as an environment gap that should not be quietly papered over
