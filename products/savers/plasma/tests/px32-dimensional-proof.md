## Purpose

This note records the actual evidence surface for the bounded `PX32` dimensional subset beyond `heightfield`.

## Commands And Harness Steps Actually Used

Commands run in this checkout:

- `python tools/scripts/check_docs_basics.py`
- `python tools/scripts/plasma_lab.py validate`
- `python tools/scripts/check_sdk_surface.py products/savers/plasma/packs/lava_remix`
- `git diff --check`

Proof inputs reviewed and extended in-repo:

- `products/savers/plasma/src/plasma_presentation.c`
- `products/savers/plasma/src/plasma_premium.c`
- `products/savers/plasma/src/plasma_benchlab.c`
- `products/savers/plasma/src/plasma_validate.c`
- `products/savers/plasma/tests/smoke.c`

## Dimensional Modes Exercised

The checked-in smoke surface now asserts:

- `aurora_curtain` resolves to bounded `curtain`
- `ribbon_aurora` resolves to bounded `ribbon`
- `substrate_relief` resolves to bounded `bounded_surface`
- `filament_extrusion` resolves to bounded `contour_extrusion`
- lower-lane degrade still drops non-flat presentation back to `flat`
- BenchLab forcing now accepts bounded non-flat requests beyond `heightfield`
- the validation catalog now records `dimensional_presentation_subset` and `dimensional_presentation_subset_bounded`

## Degrade And Fallback Behavior Exercised

- Premium-capable dimensional paths are admitted only on `gl46`.
- Lower-lane or unsupported premium presentation requests degrade honestly back to `flat`.
- Unsupported broader presentation breadth remains outside the admitted subset and is not claimed as running.

## Lane And Capability Notes

- The admitted non-flat subset remains premium-only.
- `contour_extrusion` is bounded to contour output plans.
- `bounded_billboard_volume` remains unsupported and is not claimed as part of the current subset.

## What Passed, Failed, Or Was Blocked

Passed in repo state:

- source-level smoke assertions for the bounded new dimensional subset
- BenchLab parser, snapshot, and report truth for the admitted presentation vocabulary
- validation-catalog updates for the dimensional subset and bounded-known-limit posture

Blocked in this checkout:

- no local compiler toolchain was available on `PATH`
- no fresh compiled Plasma smoke binary rerun or rendered dimensional capture generation was available

## Actual Evidence Versus Documentation Only

Actual evidence in this checkout:

- checked-in runtime implementation
- checked-in smoke assertions
- checked-in BenchLab and validation truth surfaces

Documentation-only in this checkout:

- any claim that a fresh compiled local rerun exercised the updated dimensional subset end to end
