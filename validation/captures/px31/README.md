# PX31 Capture Surface

`PX31` is primarily a bounded glyph-and-field-language tranche.
Its most truthful evidence in this checkout is the checked-in runtime code, smoke assertions, BenchLab text-surface expectations, and the PX31 proof notes.

## Commands Used

- `python tools/scripts/check_docs_basics.py`
- `python tools/scripts/plasma_lab.py validate`
- `git diff --check`

## Current Evidence Shape

Current `PX31` evidence is:

- runtime implementation for a bounded glyph subset
- runtime implementation for a bounded field-families-I subset
- updated smoke assertions
- updated validation catalog entries
- updated BenchLab text-surface expectations

## Why No Generated Output Files Are Checked In

This checkout still does not have a local compiler toolchain or prebuilt Plasma smoke binary, so no fresh compiled output or rendered capture can be generated honestly here.
Checking in fabricated output files would violate repo rules.

The checked-in `PX31` proof therefore stays in:

- the runtime code
- the smoke surface
- the validation catalog
- the PX31 proof notes
