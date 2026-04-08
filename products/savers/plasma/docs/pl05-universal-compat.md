# Plasma PL05: Universal And Compat Baseline

Status: lower-band hardening and proof phase after `PL04`.

## Purpose

`PL05` makes the refactored Plasma engine honest and durable on its guaranteed lower bands:

- `universal -> gdi`
- `compat -> gl11`

It does this without broadening Plasma into later `PL06+` work and without changing the preserved Plasma Classic product surface.

## Relationship To PL00 Through PL04

- [`pl00-constitution.md`](./pl00-constitution.md) froze lower-band truth as a constitutional requirement.
- [`pl01-taxonomy.md`](./pl01-taxonomy.md) and [`pl01-schema.md`](./pl01-schema.md) froze the vocabulary PL05 now validates against.
- [`pl02-plasma-classic.md`](./pl02-plasma-classic.md) preserved the current real saver as `Plasma Classic`.
- [`pl03-engine-core.md`](./pl03-engine-core.md) introduced the compiled runtime plan and execution-state split.
- [`pl04-output-framework.md`](./pl04-output-framework.md) made output, treatment slots, and presentation explicit internal stages.

`PL05` takes that refactored shape and proves the preserved classic path still stands on `gdi` and `gl11`.

## What “Universal” Means For Plasma

For Plasma, `universal` means the preserved classic/default path remains genuinely usable on the mandatory `gdi` floor.

In practical PL05 terms that means:

- CPU field generation remains the baseline
- classic preset/theme identity still resolves and executes
- the runtime path stays `raster` plus `native_raster` plus `flat`
- treatment slots stay on explicit safe defaults
- no hidden dependency on `gl11+`
- no dependency on advanced-only post or presentation features

## What “Compat” Means For Plasma

For Plasma, `compat` means the same classic/default product identity remains valid on the conservative accelerated `gl11` lane.

In PL05 terms that means:

- the same classic content identity is preserved
- the same preset, theme, and alias invariants remain valid
- the same default output/treatment/presentation path is used
- `gl11` does not become an excuse to widen Plasma into a richer `gl21+` feature lane

## Current Default Classic Path On The Lower Bands

The preserved lower-band classic/default path is:

- output family: `raster`
- output mode: `native_raster`
- sampling treatment: `none`
- filter treatment: `none`
- emulation treatment: `none`
- accent treatment: `none`
- presentation mode: `flat`

This is the current honest implementation envelope on both `gdi` and `gl11`.

## Preserved Routing And Manifest Posture

`PL05` preserves the current routing posture exactly:

- `minimum_kind=gdi`
- `preferred_kind=gl11`
- `quality_class=safe`

The product still truthfully centers on `gdi` plus `gl11`.
`gl21` is not promoted into the stable baseline, either publicly or implicitly.

## Lower-Band Allowed Feature Envelope

The current lower-band envelope is intentionally narrow:

- classic effect families only
- preserved classic preset and theme inventory
- classic alias preservation
- raster output only
- flat presentation only
- explicit no-op treatment slots only
- classic simulation smoothing preserved where it already lived before PL04

This is sufficient for an honest lower-band Plasma baseline and intentionally not a preview of later flagship breadth.

## Unsupported-Feature And Unsupported-Combination Policy

PL05 freezes this lower-band policy:

- unsupported output families or modes fail lower-band validation
- unsupported non-default treatment values fail lower-band validation
- unsupported non-flat presentation values fail lower-band validation
- unsupported renderer kinds do not count as part of the PL05 lower-band proof

The new product-local lower-band validation seam exists to make these decisions explicit instead of leaving them implicit in scattered render-path assumptions.

## Validation And Proof Strategy Used In PL05

PL05 adds a small validation module:

- [`../src/plasma_validate.h`](../src/plasma_validate.h)
- [`../src/plasma_validate.c`](../src/plasma_validate.c)

That module now owns:

- lower-band renderer-kind resolution
- explicit `gdi` and `gl11` lower-band recognition
- validation that the current plan stays within the preserved lower-band classic envelope
- per-renderer-kind acceptance checks for `gdi` and `gl11`

The lower-band proof work recorded in [`../tests/pl05-lower-band-proof.md`](../tests/pl05-lower-band-proof.md) rebuilt the saver, rebuilt the smoke executable, and ran the smoke executable successfully.

## Current Known Limitations

- PL05 proves the lower-band baseline through plan validation, session allocation, and framework-stage execution in the smoke path.
- PL05 does not claim a new capture-diff harness or live interactive backend-comparison workflow.
- No `gl21`, `gl33`, or `gl46` feature support is claimed here.
- No new public output, treatment, or presentation settings surface is added here.

## What Remained Intentionally Unchanged

`PL05` intentionally does not change:

- the Plasma product name
- the Plasma Classic preserved inventory
- current preset and theme defaults
- current aliases
- current public settings surfaces
- current pack surface
- current public renderer law
- current lack of higher-band public feature claims

## What PL06 May Build On Next

`PL06` may build on this lower-band baseline by expanding authored content and content plumbing.

What it must not need to reopen:

- the preserved classic/default path
- the explicit lower-band routing posture
- the explicit `gdi` and `gl11` validation envelope frozen here

## Scope Statement

`PL05` is a universal/compat hardening and proof phase, not a higher-band feature expansion.
It leaves Plasma with an explicit, documented, and test-backed lower-band baseline while preserving the existing Plasma Classic identity.
