# PX30 Treatment Proof

## Purpose

This note records the actual treatment-surface evidence for the bounded PX30 treatment-family subset.

## Commands And Harness Steps Actually Used

Commands run in this checkout:

- `python tools/scripts/check_docs_basics.py`
- `git diff --check`

Proof inputs reviewed and extended in-repo:

- `products/savers/plasma/src/plasma_treatment.c`
- `products/savers/plasma/src/plasma_advanced.c`
- `products/savers/plasma/src/plasma_modern.c`
- `products/savers/plasma/src/plasma_validate.c`
- `products/savers/plasma/tests/smoke.c`

## Treatment Families Exercised

The checked-in smoke surface now asserts the bounded subset for:

- `glow_edge`
- `halftone_stipple`
- `emboss_edge`
- `phosphor`
- `crt`
- `accent_pass`

It also preserves the existing bounded richer-lane assertions for:

- `blur`
- `overlay_pass`

## Stable And Experimental Subset Exercised

Stable default behavior preserved:

- `raster` plus `native_raster`
- no-op treatment default

Experimental subset asserted:

- the new PX30 treatment families above
- BenchLab reporting of treatment-slot truth for experimental presets

## What Passed, Failed, Or Was Blocked

Passed in repo state:

- source-level smoke assertions for the bounded treatment subset
- validation-catalog updates for `treatment_family_subset`
- BenchLab snapshot, overlay, and report assertions for experimental treatment truth

Blocked in this checkout:

- no local compiler toolchain was available on `PATH`
- no fresh compiled Plasma smoke binary rerun or rendered capture generation was available

## Actual Evidence Versus Documentation Only

Actual evidence in this checkout:

- checked-in runtime implementation
- checked-in smoke assertions
- checked-in BenchLab-report assertions

Documentation-only in this checkout:

- any claim that a fresh compiled local rerun exercised the updated treatment subset end to end

## Current Conclusion

`PX30` makes a curated treatment subset real and explicitly experimental, but the local compiled rerun remains blocked in this checkout.
