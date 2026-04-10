# PX40 Capture Surface

`PX40` is primarily a CLI-first authoring and ops tranche.
Its most truthful evidence in this checkout is the expanded Lab tool, the checked-in authored files, and existing BenchLab text captures that the new commands inspect.

## Commands Used

- `python tools/scripts/plasma_lab.py degrade-report --capture validation/captures/pl13/benchlab-plasma-degrade-heightfield-auto.txt`
- `python tools/scripts/plasma_lab.py capture-diff --left validation/captures/pl13/benchlab-plasma-gdi.txt --right validation/captures/pl13/benchlab-plasma-gl11.txt`
- `git diff --check`

## Current Evidence Shape

Current `PX40` evidence is:

- expanded CLI-first Plasma Lab tooling
- checked-in authored files and pack provenance
- semantic inspection over existing BenchLab text captures
- updated PX40 proof notes

## Why No New Generated Output Files Are Checked In

This checkout still does not have a local compiler toolchain or fresh compiled Plasma capture harness available.
`PX40` therefore uses the truthful existing BenchLab text-capture surface rather than fabricating new rendered output.

The checked-in PX40 proof stays in:

- `tools/scripts/plasma_lab.py`
- the existing `validation/captures/pl13/` BenchLab text captures
- the PX40 proof notes
