# PX31 Glyph Proof

## Purpose

This note records the actual glyph-surface evidence for the bounded PX31 glyph subset.

## Commands And Harness Steps Actually Used

Commands run in this checkout:

- `python tools/scripts/check_docs_basics.py`
- `python tools/scripts/plasma_lab.py validate`
- `git diff --check`

Proof inputs reviewed and extended in-repo:

- `products/savers/plasma/src/plasma_output.c`
- `products/savers/plasma/src/plasma_treatment.c`
- `products/savers/plasma/src/plasma_presets.c`
- `products/savers/plasma/src/plasma_validate.c`
- `products/savers/plasma/tests/smoke.c`

## Glyph, ASCII, And Matrix Paths Exercised

The checked-in smoke surface now asserts:

- `ascii_reactor` resolves to `glyph` plus `ascii_glyph`
- `matrix_lattice` resolves to `glyph` plus `matrix_glyph`
- both presets produce non-black treated output through the real session path
- BenchLab truth now reports `glyph`, `matrix_glyph`, and `lattice_quasi_crystal` for the bounded Matrix preset

## What Passed, Failed, Or Was Blocked

Passed in repo state:

- source-level smoke assertions for the bounded glyph subset
- validation-catalog updates for `glyph_output_subset` and `glyph_subset`
- BenchLab snapshot and report assertions for experimental glyph truth

Blocked in this checkout:

- no local compiler toolchain was available on `PATH`
- no fresh compiled Plasma smoke binary rerun or rendered glyph capture generation was available

## Actual Evidence Versus Documentation Only

Actual evidence in this checkout:

- checked-in runtime implementation
- checked-in smoke assertions
- checked-in BenchLab text-surface expectations

Documentation-only in this checkout:

- any claim that a fresh compiled local rerun exercised the updated glyph subset end to end

## Current Conclusion

`PX31` makes a bounded glyph subset real and explicitly experimental, but the local compiled rerun remains blocked in this checkout.
