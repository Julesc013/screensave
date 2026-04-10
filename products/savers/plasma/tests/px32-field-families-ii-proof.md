## Purpose

This note records the actual evidence surface for the bounded `PX32` field-families-II subset.

## Commands And Harness Steps Actually Used

Commands run in this checkout:

- `python tools/scripts/check_docs_basics.py`
- `python tools/scripts/plasma_lab.py validate`
- `python tools/scripts/check_sdk_surface.py products/savers/plasma/packs/lava_remix`
- `git diff --check`

Proof inputs reviewed and extended in-repo:

- `products/savers/plasma/src/plasma_presets.c`
- `products/savers/plasma/src/plasma_content.c`
- `products/savers/plasma/src/plasma_sim.c`
- `products/savers/plasma/src/plasma_plan.c`
- `products/savers/plasma/src/plasma_validate.c`
- `products/savers/plasma/tests/smoke.c`

## Field-Families-II Paths Exercised

The checked-in smoke surface now asserts:

- `aurora_curtain` resolves to the bounded `aurora_curtain_ribbon` family on a premium-capable curtain path
- `ribbon_aurora` resolves to the same family on a bounded premium ribbon path
- `substrate_relief` resolves to `substrate_vein_coral` on a bounded premium surface path
- `filament_extrusion` resolves to `arc_discharge` on a bounded premium contour-extrusion path
- the validation catalog now records `field_family_ii_subset` and `field_family_ii_subset_bounded`

## Per-Family Or Per-Lane Notes

- `aurora_curtain_ribbon` is proved only for the bounded preset-driven subset, not for scenic or broader atmospheric rendering.
- `substrate_vein_coral` is proved only as an abstract field study, not as literal biology or ecology.
- `arc_discharge` is proved only as a bounded abstract discharge study, not as an object-demo electric scene.
- Lower lanes remain truthful through degrade back to flat presentation.

## What Passed, Failed, Or Was Blocked

Passed in repo state:

- source-level smoke assertions for the bounded new field-family subset
- validation-catalog updates for `field_family_ii_subset`
- BenchLab text-surface expectations for the admitted PX32 generator-family names

Blocked in this checkout:

- no local compiler toolchain was available on `PATH`
- no fresh compiled Plasma smoke binary rerun or rendered capture generation was available

## Actual Evidence Versus Documentation Only

Actual evidence in this checkout:

- checked-in runtime implementation
- checked-in smoke assertions
- checked-in validation and BenchLab truth surfaces

Documentation-only in this checkout:

- any claim that a fresh compiled local rerun exercised the updated field-family-II subset end to end
