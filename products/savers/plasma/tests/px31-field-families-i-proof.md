# PX31 Field Families I Proof

## Purpose

This note records the actual field-families-I evidence for the bounded PX31 generator subset.

## Commands And Harness Steps Actually Used

Commands run in this checkout:

- `python tools/scripts/check_docs_basics.py`
- `python tools/scripts/plasma_lab.py validate`
- `git diff --check`

Proof inputs reviewed and extended in-repo:

- `products/savers/plasma/src/plasma_sim.c`
- `products/savers/plasma/src/plasma_config.c`
- `products/savers/plasma/src/plasma_presets.c`
- `products/savers/plasma/src/plasma_validate.c`
- `products/savers/plasma/tests/smoke.c`

## Field-Families-I Paths Exercised

The checked-in smoke surface now asserts:

- `ascii_reactor` resolves to `chemical_cellular_growth`
- `matrix_lattice` resolves to `lattice_quasi_crystal`
- `cellular_bloom` resolves to the bounded chemical-plus-raster-plus-glow path
- `quasi_crystal_bands` resolves to the bounded lattice-plus-banded-plus-accent path
- `caustic_waterlight` resolves to the bounded caustic-plus-contour-plus-glow path

## Per-Family And Per-Lane Notes

- `chemical_cellular_growth` is exercised on the lower-band glyph and raster subset.
- `lattice_quasi_crystal` is exercised on the lower-band glyph and banded subset.
- `caustic_marbling` is exercised on the lower-band contour subset.
- No broader lane sweep or premium-only widening is claimed for these new families.

## What Passed, Failed, Or Was Blocked

Passed in repo state:

- source-level smoke assertions for the bounded field-families-I subset
- validation-catalog updates for `field_family_i_subset`
- BenchLab generator-family truth for the bounded subset

Blocked in this checkout:

- no local compiler toolchain was available on `PATH`
- no fresh compiled Plasma smoke binary rerun or rendered field-family capture generation was available

## Actual Evidence Versus Documentation Only

Actual evidence in this checkout:

- checked-in simulation changes
- checked-in preset and content wiring
- checked-in smoke assertions

Documentation-only in this checkout:

- any claim that a fresh compiled local rerun exercised the updated field-family subset end to end

## Current Conclusion

`PX31` makes a bounded field-families-I subset real and explicitly experimental, but the local compiled rerun remains blocked in this checkout.
