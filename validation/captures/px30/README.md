# PX30 Capture Surface

`PX30` is primarily a bounded output-and-treatment grammar tranche.
Its most truthful evidence in this checkout is the checked-in runtime code, smoke assertions, BenchLab text-surface expectations, and the PX30 proof notes.

## Commands Used

- `python tools/scripts/check_docs_basics.py`
- `git diff --check`

## Current Evidence Shape

Current `PX30` evidence is:

- runtime implementation for bounded contour and banded output
- runtime implementation for a bounded treatment subset
- updated smoke assertions
- updated validation catalog entries
- updated BenchLab text-surface expectations

## Why No Generated Output Files Are Checked In

This checkout still does not have a local compiler toolchain or prebuilt Plasma smoke binary, so no fresh compiled output or rendered capture can be generated honestly here.
Checking in fabricated output files would violate repo rules.

The checked-in `PX30` proof therefore stays in:

- the runtime code
- the smoke surface
- the validation catalog
- the PX30 proof notes
