# PX40 Stable Vs Experimental Notes

## Purpose

This note records the stable-versus-experimental posture for the new `PX40` authoring and ops tooling surfaces.

## Newly Introduced Tooling Surfaces

`PX40` broadens the current `plasma_lab.py` shell with:

- `authoring-report`
- richer `validate`
- richer `compare`
- `compat-report`
- `migration-report`
- capture-aware `degrade-report`
- `capture-diff`

## Stable Tooling Surfaces

The following tooling surfaces are stable:

- `validate`
- `authoring-report`
- authored-object `compare`
- `compat-report`
- `migration-report`
- pack-shell `degrade-report --pack`

## Why Those Surfaces Are Stable

They are stable because they are:

- read-only
- tied to current repo-grounded authored and compiled product truth
- supportable on the current checked-in substrate
- not dependent on richer experimental rendering claims

## Experimental Tooling Surfaces

The following tooling surfaces remain experimental:

- capture-backed `degrade-report --capture`
- `capture-diff`

## Why Those Surfaces Stay Experimental

They remain experimental because they depend on:

- existing BenchLab text capture shape
- bounded semantic comparison over report fields rather than universal raw-text or rendered-pixel determinism
- a capture and proof surface that is still narrower than a broad cross-lane or cross-driver automation lab

## Narrow Posture Update

`PX40` adds a stronger authoring and ops toolchain, but it does not widen Plasma's stable visual-language promise.
The default product path remains the same stable Classic-first and lower-band-first path as before.

## Explicit Stable-Widening Statement

No stable widening of the shipped Plasma runtime or content pool occurred in `PX40`.
The stable additions in this tranche are support tooling only.
