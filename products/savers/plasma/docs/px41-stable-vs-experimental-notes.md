# PX41 Stable Vs Experimental Notes

## Purpose

This note records the stable-versus-experimental posture for the new `PX41` integration, curation, and automation surfaces.

## Newly Introduced Surfaces

`PX41` introduces:

- projection metadata for `anthology` and `suite`
- local SDK/reference metadata
- bounded local control profiles
- curated collections
- a local provenance index
- `integration-report`
- `control-report`
- `curation-report`

## Stable Surfaces

The following surfaces may be treated as stable support surfaces now:

- `classic_core_featured`
- `dark_room_support`
- `anthology_stable_core`
- `suite_stable_dark_room`
- `deterministic_classic`
- `dark_room_support` control profile
- the `lava_remix_builtin` provenance index entry
- the read-only report commands `integration-report`, `control-report`, and `curation-report` when used against the stable metadata admitted above

### Rationale

These surfaces are stable because they only restate or inspect already-stable Plasma meaning:

- the preserved classic core
- the quieter dark-room support slice
- the shipped `lava_remix` pack provenance
- existing stable routing and support posture

They do not widen the stable runtime or stable content promise.

## Experimental Surfaces

The following surfaces remain explicitly experimental:

- `wave3_experimental_sampler`
- `anthology_experimental_studies`
- `suite_experimental_sampler`
- `experimental_matrix`
- any report output that references those experimental surfaces

### Rationale

These surfaces remain experimental because they expose bounded Wave 3 study material that is still opt-in and not part of the current stable Plasma core.

## Narrow Posture Update

`PX41` adds stronger local ecosystem metadata and stronger local support-facing reports.
It does not widen the stable default Plasma runtime path.

## Scope Boundary

No hidden stable widening occurred.
`PX41` changes only how Plasma records and reports bounded local integration and curation meaning.
